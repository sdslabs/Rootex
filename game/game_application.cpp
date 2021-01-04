#include "game_application.h"

#include "core/input/input_manager.h"
#include "core/resource_loader.h"
#include "framework/scene_loader.h"
#include "framework/systems/audio_system.h"
#include "framework/systems/render_system.h"
#include "framework/systems/render_ui_system.h"
#include "framework/systems/ui_system.h"
#include "framework/systems/physics_system.h"
#include "framework/systems/transform_animation_system.h"
#include "systems/debug_system.h"

#include "systems/game_render_system.h"

Ref<Application> CreateRootexApplication()
{
	return Ref<Application>(new GameApplication());
}

String GameApplication::getLevelNameFromCommandLine(const char* s)
{
	/// https://github.com/wine-mirror/wine/blob/7ec5f555b05152dda53b149d5994152115e2c623/dlls/shell32/shell32_main.c#L58
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

	return String(s);
}

Variant GameApplication::onExitEvent(const Event* event)
{
	PostQuitMessage(0);
	return true;
}

GameApplication::GameApplication()
    : Application("game/game.app.json")
{
	String levelName = getLevelNameFromCommandLine(GetCommandLine());

	if (levelName.empty())
	{
		SceneLoader::GetSingleton()->loadScene(m_ApplicationSettings->getJSON()["startLevel"], {});
	}
	else
	{
		SceneLoader::GetSingleton()->loadScene(levelName, {});
	}

	GameRenderSystem::GetSingleton()->initialize({});
	DebugSystem::GetSingleton()->initialize(m_ApplicationSettings->getJSON()["systems"]["DebugSystem"]);
}
