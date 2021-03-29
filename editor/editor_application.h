#pragma once

#include "rootex/app/application.h"

class EditorApplication : public Application
{
	static EditorApplication* s_Instance;
	static void SetSingleton(EditorApplication* app);

	unsigned int m_AutoSaveDurationS = 300.0f;

	TimePoint m_PointAtLast10Second;
	FrameTimer m_FrameTimer;

public:
	static EditorApplication* GetSingleton();

	EditorApplication();
	EditorApplication(EditorApplication&) = delete;
	~EditorApplication();

	virtual void process(float deltaMilliseconds) override;
};
