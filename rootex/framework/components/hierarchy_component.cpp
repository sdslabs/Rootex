#include "hierarchy_component.h"

Component* HierarchyComponent::Create(const LuaVariable& componentData)
{
	HierarchyComponent* component = new HierarchyComponent();
	return component;
}

bool HierarchyComponent::addChild(Ref<Entity> child)
{
	m_Children.push_back(child);
	child->getComponent<HierarchyComponent>()->m_Parent = this->m_Owner;
	return true;
}

bool HierarchyComponent::removeChild(Ref<Entity> node)
{
	auto& findIt = std::find(m_Children.begin(), m_Children.end(), node);
	if (findIt != m_Children.end())
	{
		(*findIt)->getComponent<HierarchyComponent>()->m_Parent = nullptr;
		m_Children.erase(findIt);
	}
	return true;
}
