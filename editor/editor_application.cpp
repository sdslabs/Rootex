#include "editor_application.h"

#include "editor.h"

#include "rootex/core/audio/audio_system.h"
#include "rootex/core/input/input_manager.h"
#include "rootex/core/resource_loader.h"
#include "rootex/framework/systems/render_system.h"

Ref<Application> CreateRootexApplication()
{
	return Ref<Application>(new EditorApplication());
}

EditorApplication::EditorApplication()
    : Application("editor/project.lua")
{
	Editor::GetSingleton()->initialize(m_Window->getWindowHandle());

	addEntity("game/assets/test/cube.lua");
}

EditorApplication::~EditorApplication()
{
}

void EditorApplication::run()
{
	while (true)
	{
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
