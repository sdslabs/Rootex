#include "entity.h"

#include "framework/component.h"
#include "framework/components/hierarchy_component.h"
#include "framework/system.h"
#include "event_manager.h"

Entity::~Entity()
{
	destroy();
}

void Entity::addComponent(const Ref<Component>& component)
{
	m_Components.insert(std::make_pair(component->getComponentID(), component));
}

void Entity::addChild(Ref<Entity> child)
{
	getComponent<HierarchyComponent>()->addChild(child);
}

Entity::Entity(EntityID id, const String& name)
    : m_ID(id)
    , m_Name(name)
{
}

bool Entity::setupComponents()
{
	bool status = true;
	for (auto& component : m_Components)
	{
		status = status & component.second->setup();
	}
	return status;
}

void Entity::destroy()
{
	for (auto& component : m_Components)
	{
		System::DeregisterComponent(component.second.get());
		component.second.reset();
	}
	m_Components.clear();
}

void Entity::removeComponent(Ref<Component> component)
{
	m_Components.erase(component->getComponentID());
	System::DeregisterComponent(component.get());
	component.reset();
}

EntityID Entity::getID() const
{
	return m_ID;
}

const String& Entity::getName() const
{
	return m_Name;
}

bool Entity::hasComponent(ComponentID componentID)
{
	return m_Components.find(componentID) != m_Components.end();
}

void Entity::setName(const String& name)
{
	m_Name = name;
}

const HashMap<ComponentID, Ref<Component>>& Entity::getAllComponents() const
{
	return m_Components;
}
