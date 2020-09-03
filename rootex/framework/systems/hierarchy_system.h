#pragma once

#include "framework/system.h"
#include "components/hierarchy_component.h"
#include "components/hierarchy_graph.h"

/// Generates hierarchy system out of hierarchy graph, entities and components.
class HierarchySystem : public System
{
	HierarchyGraph m_HierarchyGraph;

	HierarchySystem();

public:
	static HierarchySystem* GetSingleton();

	/// Adds child entity to root hierarchy component of hierarchy graph.
	void addChild(Ref<Entity> child);

	/// Points to Root Entity of hierarchy graph.
	Ref<Entity> getRootEntity() const { return m_HierarchyGraph.getRootEntity(); }
	/// Points to Root Hierarchy Component.
	Ref<HierarchyComponent> getRootHierarchyComponent() const { return m_HierarchyGraph.getRootHierarchyComponent(); }
	HierarchyGraph* getHierarchyGraph() { return &m_HierarchyGraph; }
};
