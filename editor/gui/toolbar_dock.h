#pragma once

class ToolbarDock
{
public:
	struct ToolbarDockSettings
	{
		bool m_IsActive = true;
	};

private:
	ToolbarDockSettings m_ToolbarDockSettings;

public:
	ToolbarDock() = default;
	ToolbarDock(ToolbarDock&) = delete;
	~ToolbarDock() = default;

	void draw();

	ToolbarDockSettings& getSettings() { return m_ToolbarDockSettings; }
	void setActive(bool enabled) { m_ToolbarDockSettings.m_IsActive = enabled; }
};
