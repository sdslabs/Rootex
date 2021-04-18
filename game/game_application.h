#pragma once

#include "rootex/app/application.h"
#include "framework/systems/script_system.h"

/// Application that runs when game is run without the editor
class GameApplication : public Application
{
	Optional<String> getSceneNameFromCommandLine(const char* s);
	Variant onExitEvent(const Event* event);

public:
	GameApplication();
	GameApplication(GameApplication&) = delete;
	~GameApplication() = default;
};
