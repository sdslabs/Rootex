#pragma once

class HierarchyDock
{
public:
	struct HierarchySettings
	{
		bool m_IsActive = true;
	};

private:
	HierarchySettings m_HierarchySettings;

public:
	HierarchyDock() = default;
	HierarchyDock(HierarchyDock&) = delete;
	~HierarchyDock() = default;

	void draw();

	HierarchySettings& getSettings() { return m_HierarchySettings; }
	void setActive(bool enabled) { m_HierarchySettings.m_IsActive = enabled; }
};
