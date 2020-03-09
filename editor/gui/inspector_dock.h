#pragma once

class InspectorDock
{
public:
	struct InspectorSettings
	{
		bool m_IsActive = true;
	};

private:
	InspectorSettings m_InspectorSettings;

public:
	InspectorDock() = default;
	InspectorDock(InspectorDock&) = delete;
	~InspectorDock() = default;

	void draw();

	InspectorSettings& getSettings() { return m_InspectorSettings; }
	void setActive(bool enabled) { m_InspectorSettings.m_IsActive = enabled; }
};
