#pragma once

#include "common/common.h"
#include "entity_factory.h"
#include "components/hierarchy_component.h"

class HierarchyGraph
{
protected:
	Ref<Entity> m_Root;
	Ref<HierarchyComponent> m_RootHierarchyComponent;

public:
	HierarchyGraph();
	HierarchyGraph(HierarchyGraph&) = delete;
	~HierarchyGraph();
	
	bool addChild(Ref<Entity> child);
	bool removeChild(Ref<Entity> child);

	Ref<Entity> getRootEntity() const { return m_Root; }
	Ref<HierarchyComponent> getRootHierarchyComponent() const { return m_RootHierarchyComponent; }
};
