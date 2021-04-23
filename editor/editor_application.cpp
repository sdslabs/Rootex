#include "editor_application.h"

#include "editor_system.h"

#include "rootex/core/resource_loader.h"
#include "rootex/core/ui/input_interface.h"
#include "rootex/framework/systems/render_system.h"
#include "rootex/framework/systems/render_ui_system.h"
#include "rootex/framework/systems/ui_system.h"
#include "rootex/framework/systems/script_system.h"
#include "rootex/framework/systems/physics_system.h"
#include "rootex/framework/systems/audio_system.h"
#include "rootex/framework/systems/input_system.h"
#include "rootex/framework/systems/player_system.h"

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
    : Application("RootexEditor", "editor/editor.app.json")
{
	if (!s_Instance)
	{
		SetSingleton(this);
	}
	else
	{
		ERR("More than 1 instances of Editor Application detected");
	}

	EditorSystem::GetSingleton()->initialize(m_ApplicationSettings->getJSON()["systems"]["EditorSystem"]);
	setGameMode(false);

	destroySplashWindow();

	loadSave(0);

	m_PointAtLast10Second = m_ApplicationTimer.Now();
}

EditorApplication::~EditorApplication()
{
	saveSlot();
}

void EditorApplication::setGameMode(bool enabled)
{
	PhysicsSystem::GetSingleton()->setActive(enabled);
	AudioSystem::GetSingleton()->setActive(enabled);
	ScriptSystem::GetSingleton()->setActive(enabled);
	PlayerSystem::GetSingleton()->setActive(enabled);
	RenderSystem::GetSingleton()->setIsEditorRenderPass(!enabled);
	InputSystem::GetSingleton()->flushSchemes();
	InputSystem::GetSingleton()->loadSchemes(m_ApplicationSettings->getJSON()["systems"]["InputSystem"]["inputSchemes"]);
	InputSystem::GetSingleton()->pushScheme(m_ApplicationSettings->getJSON()["systems"]["InputSystem"]["startScheme"]);
	InputInterface::GetSingleton()->m_IsEnabled = enabled;
	m_IsAutoSave = !enabled;

	EventManager::GetSingleton()->call(EditorEvents::EditorReset);
}

void EditorApplication::process(float deltaMilliseconds)
{
	if (m_IsAutoSave && ((m_ApplicationTimer.Now() - m_PointAtLast10Second).count()) * NS_TO_MS * MS_TO_S > m_AutoSaveDurationS)
	{
		EventManager::GetSingleton()->call(EditorEvents::EditorAutoSave);
		m_PointAtLast10Second = m_ApplicationTimer.Now();
	}
}
