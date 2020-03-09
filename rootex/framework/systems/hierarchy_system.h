#pragma once

#include "framework/system.h"
#include "components/hierarchy_component.h"
#include "components/hierarchy_graph.h"

class HierarchySystem : public System
{
	HierarchyGraph m_HierarchyGraph;

public:
	static HierarchySystem* GetSingleton();

	void addChild(Ref<Entity> child);

	RootHierarchyComponent* getRoot() const { return m_HierarchyGraph.getRoot(); }
	HierarchyGraph* getHierarchyGraph() { return &m_HierarchyGraph; }
};
