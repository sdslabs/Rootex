#pragma once

#include "rootex/app/application.h"
#include "rootex/framework/systems/hierarchy_system.h"
#include "rootex/script/interpreter.h"

class HierarchyGraph;

class GameApplication : public Application
{
	FrameTimer m_FrameTimer;

	Variant onExitEvent(const Event* event);

public:
	GameApplication();
	GameApplication(GameApplication&) = delete;
	~GameApplication();
	
	void run() override;
	void shutDown() override;

	String getAppTitle() const override { return "Game"; }
};
