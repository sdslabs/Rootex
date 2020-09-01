#include "editor_application.h"

#include "editor.h"

#include "rootex/framework/systems/audio_system.h"
#include "rootex/core/input/input_manager.h"
#include "rootex/core/resource_loader.h"
#include "rootex/framework/systems/render_system.h"
#include "rootex/framework/systems/render_ui_system.h"
#include "rootex/framework/systems/ui_system.h"
#include "rootex/framework/systems/physics_system.h"
#include "rootex/framework/systems/transform_animation_system.h"

EditorApplication* EditorApplication::s_Instance = nullptr;

Ref<Application> CreateRootexApplication()
{
	return Ref<Application>(new EditorApplication());
}

EditorApplication* EditorApplication::GetSingleton()
{
	return s_Instance;
}

void EditorApplication::SetSingleton(EditorApplication* app)
{
	s_Instance = app;
}

EditorApplication::EditorApplication()
    : Application("editor/editor.app.json")
{
	if (!s_Instance)
	{
		SetSingleton(this);
	}
	else
	{
		ERR("More than 1 instances of Editor Application detected");
	}

	InputManager::GetSingleton()->loadSchemes(getSettings()->getJSON()["inputSchemes"]);
	InputManager::GetSingleton()->setScheme(getSettings()->getJSON()["startScheme"]);

	RenderSystem::GetSingleton()->setIsEditorRenderPass(true);
	RenderUISystem::GetSingleton();
	Editor::GetSingleton()->initialize(m_Window->getWindowHandle(), m_ApplicationSettings->getJSON());
	m_PointAtLast10Second = m_ApplicationTimer.Now();
}

EditorApplication::~EditorApplication()
{
}

void EditorApplication::run()
{
	while (true)
	{
		m_FrameTimer.reset();
		if (((m_ApplicationTimer.Now() - m_PointAtLast10Second).count()) * NS_TO_MS * MS_TO_S > m_AutoSaveDurationS)
		{
			EventManager::GetSingleton()->call("EditorAutoSaveEvent", "EditorAutoSave", 0);
			m_PointAtLast10Second = m_ApplicationTimer.Now();
		}

		if (m_Window->processMessages())
		{
			break;
		}

		UISystem::GetSingleton()->update();
		Editor::GetSingleton()->render();
		AudioSystem::GetSingleton()->update();
		InputManager::GetSingleton()->update();
		TransformAnimationSystem::GetSingleton()->update(m_FrameTimer.getFrameTime());
		EventManager::GetSingleton()->dispatchDeferred();

		m_Window->swapBuffers();
	}
}

void EditorApplication::shutDown()
{
}
