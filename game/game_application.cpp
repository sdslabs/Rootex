#include "game_application.h"

Ref<Application> CreateRootexApplication()
{
	return Ref<Application>(new GameApplication());
}

GameApplication::GameApplication()
    : Application("game/assets/config/window.lua")
{
}

GameApplication::~GameApplication()
{
}
