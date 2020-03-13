#include "hierarchy_system.h"

HierarchySystem* HierarchySystem::GetSingleton()
{
	static HierarchySystem singleton;
	return &singleton;
}

void HierarchySystem::addChild(Ref<Entity> child)
{
	m_HierarchyGraph.addChild(child);
}

void HierarchySystem::setParentAndChildren(Ref<Entity> entity, HashMap<EntityID, Ref<Entity>>& entities)
{
	HierarchyComponent* hierarchyComponent = nullptr;
	for (auto&& component : s_Components[HierarchyComponent::s_ID])
	{
		hierarchyComponent = dynamic_cast<HierarchyComponent*>(component);
		
		if (hierarchyComponent->m_ParentID != INVALID_ID)
		{
			hierarchyComponent->m_Parent = entities[hierarchyComponent->m_ParentID];
		}

		for (auto&& childID : hierarchyComponent->m_ChildrenIDs)
		{
			hierarchyComponent->m_Children.push_back(entities[childID]);
		}
	}
}

void HierarchySystem::resetHierarchy()
{
	setParentAndChildren(getRootEntity(), EntityFactory::GetSingleton()->getMutableEntities());
}
