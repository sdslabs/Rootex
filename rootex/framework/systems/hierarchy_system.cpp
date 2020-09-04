#include "hierarchy_system.h"

HierarchySystem::HierarchySystem()
    : System("HierarchySystem", UpdateOrder::None, false)
{
}

HierarchySystem* HierarchySystem::GetSingleton()
{
	static HierarchySystem singleton;
	return &singleton;
}

void HierarchySystem::addChild(Ref<Entity> child)
{
	m_HierarchyGraph.addChild(child);
}
