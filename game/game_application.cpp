#include "game_application.h"

#include "core/resource_loader.h"
#include "app/project_manager.h"
#include "core/audio/audio_system.h"
#include "core/input/input_manager.h"
#include "framework/systems/render_system.h"

Ref<Application> CreateRootexApplication()
{
	return Ref<Application>(new GameApplication());
}

Variant GameApplication::onExitEvent(const Event* event)
{
	PostQuitMessage(0);
	return true;
}

GameApplication::GameApplication()
{
	JSON::json projectJSON = JSON::json::parse(ResourceLoader::CreateNewTextResourceFile("game/game.app.json")->getString());
	initialize(projectJSON);

	ProjectManager::GetSingleton()->openLevel(FilePath((String)projectJSON["startLevel"]));
}

GameApplication::~GameApplication()
{
}

void GameApplication::run()
{
	while (true)
	{
		m_FrameTimer.reset();
		
		if (m_Window->processMessages())
		{
			break;
		}

		RenderSystem::GetSingleton()->render();
		AudioSystem::GetSingleton()->update();
		InputManager::GetSingleton()->update();
		EventManager::GetSingleton()->dispatchDeferred();

		m_Window->swapBuffers();
		m_Window->clearCurrentTarget();
	}
}

void GameApplication::shutDown()
{
}
