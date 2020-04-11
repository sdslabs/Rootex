#pragma once

#include "rootex/app/application.h"
#include "rootex/framework/systems/hierarchy_system.h"
#include "framework/systems/script_system.h"

class HierarchyGraph;

/// Application that runs when game is run without the editor
class GameApplication : public Application
{
	FrameTimer m_FrameTimer;

	String getLevelNameFromCommandLine(const char* s);

	Variant onExitEvent(const Event* event);

public:
	GameApplication();
	GameApplication(GameApplication&) = delete;
	~GameApplication();
	
	void run() override;
	void shutDown() override;

	String getAppTitle() const override { return "Game"; }
};
