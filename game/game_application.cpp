#include "game_application.h"

#include "app/project_manager.h"
#include "core/audio/audio_system.h"
#include "core/input/input_manager.h"
#include "core/resource_loader.h"
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

	// https://github.com/wine-mirror/wine/blob/7ec5f555b05152dda53b149d5994152115e2c623/dlls/shell32/shell32_main.c#L58
	char* s = GetCommandLine();
	if (*s == '"')
	{
		++s;
		while (*s)
		{
			if (*s++ == '"')
			{
				break;
			}
		}
	}
	else
	{
		while (*s && *s != ' ' && *s != '\t')
		{
			++s;
		}
	}
	/* (optionally) skip spaces preceding the first argument */
	while (*s == ' ' || *s == '\t')
		s++;

	String levelName(s);
	if (levelName == "")
	{
		ProjectManager::GetSingleton()->openLevel(projectJSON["startLevel"]);
	}
	else
	{
		ProjectManager::GetSingleton()->openLevel("game/assets/levels/" + levelName);
	}
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
