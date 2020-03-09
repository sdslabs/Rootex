#pragma once

#include "rootex/app/application.h"
#include "rootex/framework/components/visual/visual_component_graph.h"
#include "rootex/script/interpreter.h"

class VisualComponentGraph;

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
