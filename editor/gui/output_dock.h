#pragma once

#include "common/common.h"

class OutputDock
{
public:
	struct OutputDockSettings
	{
		bool m_IsActive = true;
	};

private:
	OutputDockSettings m_OutputDockSettings;
	Vector<String> m_Logs;

public:
	OutputDock() = default;
	OutputDock(OutputDock&) = delete;
	~OutputDock() = default;

	void draw();

	void log(const String& log);

	OutputDockSettings& getSettings() { return m_OutputDockSettings; }
	void setActive(bool enabled) { m_OutputDockSettings.m_IsActive = enabled; }
};
