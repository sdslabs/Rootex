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

void HierarchySystem::setParentAndChildren(HashMap<EntityID, Ref<Entity>>& entities)
{
	Vector<Ref<Entity>> rootChildren;
	for (auto&& entity : entities)
	{
		if (entity.second->getID() != ROOT_ENTITY_ID)
		{
			Ref<HierarchyComponent> hierarchyComponent = entity.second->getComponent<HierarchyComponent>();
			if (hierarchyComponent)
			{
				hierarchyComponent->m_Parent = entities.find(hierarchyComponent->m_ParentID)->second->getComponent<HierarchyComponent>().get();
				if (hierarchyComponent->m_ParentID == ROOT_ENTITY_ID)
				{
					rootChildren.push_back(entity.second);
				}

				for (auto& childID : hierarchyComponent->m_ChildrenIDs)
				{
					hierarchyComponent->m_Children.push_back(entities.find(childID)->second->getComponent<HierarchyComponent>().get());
				}
			}
		}
	}

	getRootHierarchyComponent()->clear();
	for (auto&& rootChild : rootChildren)
	{
		getRootHierarchyComponent()->addChild(rootChild);
	}
}

void HierarchySystem::resetHierarchy()
{
	setParentAndChildren(EntityFactory::GetSingleton()->getMutableEntities());
}
