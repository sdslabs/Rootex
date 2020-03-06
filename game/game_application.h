#pragma once

#include "rootex/avl/application.h"

class GameApplication : public Application
{
public:
	GameApplication();
	GameApplication(GameApplication&) = delete;
	~GameApplication();

	virtual void createGameAndView() override {}
};
