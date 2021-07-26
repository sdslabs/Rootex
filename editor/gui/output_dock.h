#pragma once

#include <iostream>
#include <sstream>

#include "common/common.h"
#include "event_manager.h"

class OutputDock
{
	enum class OutputState : unsigned int
	{
		None = 0,
		General = 1 << 0,
		Warning = 1 << 1,
		Error = 1 << 2,
		All = General | Warning | Error
	};

	EventBinder<OutputDock> m_Binder;

public:
	struct OutputDockSettings
	{
		bool m_IsActive = true;
	};

private:
	OutputDockSettings m_OutputDockSettings;
	Vector<Pair<String, String>> m_CaughtOutputs;
	bool m_IsOutputJustCaught = false;
	Variant catchOutput(const Event* event);

public:
	OutputDock();
	OutputDock(OutputDock&) = delete;
	~OutputDock() = default;

	void draw(float deltaMilliseconds);

	OutputDockSettings& getSettings() { return m_OutputDockSettings; }
	void setActive(bool enabled) { m_OutputDockSettings.m_IsActive = enabled; }
};
