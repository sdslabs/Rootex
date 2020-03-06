#pragma once

#include "rootex/avl/application.h"

class EditorApplication : public Application
{
public:
	EditorApplication();
	EditorApplication(EditorApplication&) = delete;
	~EditorApplication();

	virtual void createGameAndView() override {}
};
