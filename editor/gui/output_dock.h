#pragma once

#include <iostream>
#include <sstream>

#include "rootex/common/common.h"

// https://stackoverflow.com/a/5419388/7370948
struct CoutRedirect
{
	CoutRedirect(std::streambuf* newBuffer)
	    : m_Old(std::cout.rdbuf(newBuffer))
	{
	}

	~CoutRedirect()
	{
		std::cout.rdbuf(m_Old);
	}

private:
	std::streambuf* m_Old;
};

class OutputDock
{
	CoutRedirect m_CoutRedirect;
	std::stringstream m_StdOutBuffer;

public:
	struct OutputDockSettings
	{
		bool m_IsActive = true;
	};

private:
	OutputDockSettings m_OutputDockSettings;

public:
	OutputDock();
	OutputDock(OutputDock&) = delete;
	~OutputDock() = default;

	void draw();

	OutputDockSettings& getSettings() { return m_OutputDockSettings; }
	void setActive(bool enabled) { m_OutputDockSettings.m_IsActive = enabled; }
};
