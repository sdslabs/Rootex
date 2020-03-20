#pragma once

#include "components/hierarchy_component.h"

class HierarchyDock
{
public:
	struct HierarchySettings
	{
		bool m_IsActive = true;
	};

private:
	HierarchySettings m_HierarchySettings;
	EntityID m_OpenedEntityID;

	void showHierarchySubTree(HierarchyComponent* hierarchy);
	
	Variant selectOpenEntity(const Event* event);

public:
	HierarchyDock();
	HierarchyDock(HierarchyDock&) = delete;
	~HierarchyDock() = default;

	void draw();

	HierarchySettings& getSettings() { return m_HierarchySettings; }
	void setActive(bool enabled) { m_HierarchySettings.m_IsActive = enabled; }
};
