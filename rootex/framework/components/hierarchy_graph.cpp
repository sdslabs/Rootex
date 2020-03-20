#include "hierarchy_graph.h"

#include "resource_loader.h"

HierarchyGraph::HierarchyGraph()
{
	m_Root = EntityFactory::GetSingleton()->createRootEntity();
	m_RootHierarchyComponent = m_Root->getComponent<HierarchyComponent>();
}

HierarchyGraph::~HierarchyGraph()
{
}

bool HierarchyGraph::addChild(Ref<Entity> child)
{
	return m_RootHierarchyComponent->addChild(child);
}

bool HierarchyGraph::removeChild(Ref<Entity> child)
{
	return m_RootHierarchyComponent->removeChild(child);
}
