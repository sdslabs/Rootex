#pragma once

#include "rootex/app/application.h"
#include "systems/serialization_system.h"
#include "rootex/framework/entity_factory.h"

class EditorApplication : public Application
{
	static EditorApplication* s_Instance;
	static void SetSingleton(EditorApplication* app);

	FrameTimer m_FrameTimer;
	SerializationSystem m_SerializationSystem;

public:
	static EditorApplication* GetSingleton();

	EditorApplication();
	EditorApplication(EditorApplication&) = delete;
	~EditorApplication();

	virtual void run() override;
	virtual void shutDown() override;

	virtual String getAppTitle() const override { return "Rootex Editor"; }
	const FrameTimer& getAppFrameTimer() const { return m_FrameTimer; }
};
