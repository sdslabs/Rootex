#pragma once

#include "rootex/app/application.h"
#include "rootex/framework/entity_factory.h"

class EditorApplication : public Application
{
public:
	EditorApplication();
	EditorApplication(EditorApplication&) = delete;
	~EditorApplication();

	virtual void run() override;
	virtual void shutDown() override;

	virtual String getAppTitle() const override { return "Rootex Editor"; }
};
