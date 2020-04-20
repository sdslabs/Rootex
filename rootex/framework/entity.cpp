#include "entity.h"

#include "event_manager.h"
#include "framework/component.h"
#include "framework/components/hierarchy_component.h"
#include "framework/system.h"

Entity::~Entity()
{
	destroy();
}

void Entity::addComponent(const Ref<Component>& component)
{
	m_Components.insert(std::make_pair(component->getComponentID(), component));
}

Entity::Entity(EntityID id, const String& name, const HashMap<ComponentID, Ref<Component>>& components)
    : m_ID(id)
    , m_Name(name)
    , m_Components(components)
    , m_IsEditorOnly(false)
{
}

Component* Entity::getComponentPointer(ComponentID ID)
{
	return getComponent<Component>(ID).get();
}

JSON::json Entity::getJSON() const
{
	JSON::json j;
	j["Entity"]["name"] = getName();
	j["Entity"]["ID"] = getID();
	j["Components"] = {};
	for (auto&& [componentID, component] : m_Components)
	{
		j["Components"][component->getName()] = component->getJSON();
	}

	return j;
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
		component.second->onRemove();
		System::DeregisterComponent(component.second.get());
		component.second.reset();
	}
	m_Components.clear();
}

void Entity::removeComponent(Ref<Component> component)
{
	component->onRemove();
	m_Components.erase(component->getComponentID());
	System::DeregisterComponent(component.get());
}

EntityID Entity::getID() const
{
	return m_ID;
}

const String& Entity::getName() const
{
	return m_Name;
}

String Entity::getFullName() const
{
	return m_Name + " #" + std::to_string(getID());
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
