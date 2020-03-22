#pragma once

#include <iostream>
#include <sstream>

#include "common/common.h"
#include "event_manager.h"

class OutputDock
{
public:
	struct OutputDockSettings
	{
		bool m_IsActive = true;
	};

private:
	OutputDockSettings m_OutputDockSettings;
	Vector<Pair<String, String>> m_CaughtOutputs;

	Variant catchOutput(const Event* event);

public:
	OutputDock();
	OutputDock(OutputDock&) = delete;
	~OutputDock() = default;

	void draw();

	OutputDockSettings& getSettings() { return m_OutputDockSettings; }
	void setActive(bool enabled) { m_OutputDockSettings.m_IsActive = enabled; }
};
