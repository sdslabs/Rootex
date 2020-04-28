#pragma once

#include "components/hierarchy_component.h"

class HierarchyDock
{
public:
	struct HierarchySettings
	{
		bool m_IsActive = true;
		bool m_IsEntitiesDockActive = true;
	};

private:
	HierarchySettings m_HierarchySettings;
	EntityID m_OpenedEntityID;
	bool m_IsShowEditorEntities = false;

	void showHierarchySubTree(HierarchyComponent* hierarchy);
	void openEntity(Ref<Entity> entity);
	
	Variant selectOpenEntity(const Event* event);

public:
	HierarchyDock();
	HierarchyDock(HierarchyDock&) = delete;
	~HierarchyDock() = default;

	void draw();

	void showEntities(const HashMap<EntityID, Ref<Entity>>& entities);

	HierarchySettings& getSettings() { return m_HierarchySettings; }
	void setActive(bool enabled) { m_HierarchySettings.m_IsActive = enabled; }
};
