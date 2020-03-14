#pragma once

#include "framework/system.h"
#include "components/hierarchy_component.h"
#include "components/hierarchy_graph.h"

class HierarchySystem : public System
{
	HierarchyGraph m_HierarchyGraph;

	void setParentAndChildren(HashMap<EntityID, Ref<Entity>>& entities);

public:
	static HierarchySystem* GetSingleton();

	void addChild(Ref<Entity> child);
	void resetHierarchy();

	Ref<Entity> getRootEntity() const { return m_HierarchyGraph.getRootEntity(); }
	Ref<RootHierarchyComponent> getRootHierarchyComponent() const { return m_HierarchyGraph.getRootHierarchyComponent(); }
	HierarchyGraph* getHierarchyGraph() { return &m_HierarchyGraph; }
};
