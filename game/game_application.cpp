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
#include "systems/input_system.h"

#include "systems/game_render_system.h"

Ref<Application> CreateRootexApplication()
{
	return Ref<Application>(new GameApplication());
}

Optional<String> GameApplication::getSceneNameFromCommandLine(const char* s)
{
	String cmdLine = s;
	size_t found = cmdLine.find("game/assets/");
	if (found != String::npos)
	{
		return cmdLine.substr(found, cmdLine.size() - 1);
	}
	return {};
}

Variant GameApplication::onExitEvent(const Event* event)
{
	PostQuitMessage(0);
	return true;
}

GameApplication::GameApplication()
    : Application("RootexGame", "game/game.app.json")
{
	Optional<String> scenePath = getSceneNameFromCommandLine(GetCommandLine());

	if (scenePath)
	{
		SceneLoader::GetSingleton()->loadScene(*scenePath, {});
	}
	else
	{
		SceneLoader::GetSingleton()->loadScene(m_ApplicationSettings->getJSON()["startScene"], {});
	}

	GameRenderSystem::GetSingleton()->initialize({});
	DebugSystem::GetSingleton()->initialize(m_ApplicationSettings->getJSON()["systems"]["DebugSystem"]);
	InputSystem::GetSingleton()->flushSchemes();
	InputSystem::GetSingleton()->loadSchemes(m_ApplicationSettings->getJSON()["systems"]["InputSystem"]["inputSchemes"]);
	InputSystem::GetSingleton()->pushScheme(m_ApplicationSettings->getJSON()["systems"]["InputSystem"]["startScheme"]);
	destroySplashWindow();
}
