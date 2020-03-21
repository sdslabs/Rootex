#include "editor_application.h"

#include "editor.h"

#include "rootex/core/audio/audio_system.h"
#include "rootex/core/input/input_manager.h"
#include "rootex/core/resource_loader.h"
#include "rootex/framework/systems/render_system.h"

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
    : Application("editor/project.lua")
{
	SetSingleton(this);
	Editor::GetSingleton()->initialize(m_Window->getWindowHandle());

	HierarchySystem::GetSingleton()->addChild(addEntity("game/assets/test/cube.lua"));
	Ref<Entity> teapot = addEntity("game/assets/test/teapot.lua");
	HierarchySystem::GetSingleton()->addChild(teapot);
	teapot->addChild(addEntity("game/assets/test/point_light.lua"));
}

EditorApplication::~EditorApplication()
{
}

void EditorApplication::run()
{
	while (true)
	{
		m_FrameTimer.reset();

		if (m_Window->processMessages())
		{
			break;
		}

		Editor::GetSingleton()->render();

		AudioSystem::GetSingleton()->update();
		InputManager::GetSingleton()->update();
		EventManager::GetSingleton()->dispatchDeferred();

		m_Window->swapBuffers();
		m_Window->clearCurrentTarget();
	}
}

void EditorApplication::shutDown()
{
}
