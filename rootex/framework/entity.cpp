#include "entity.h"

#include "event_manager.h"
#include "framework/component.h"
#include "framework/components/hierarchy_component.h"
#include "framework/system.h"

void Entity::RegisterAPI(sol::table& rootex)
{
	sol::usertype<Entity> entity = rootex.new_usertype<Entity>("Entity");
	entity["removeComponent"] = &Entity::removeComponent;
	entity["destroy"] = &Entity::destroy;
	entity["hasComponent"] = &Entity::hasComponent;
	entity["getID"] = &Entity::getID;
	entity["getName"] = &Entity::getName;
	entity["setName"] = &Entity::setName;

	sol::usertype<Component> component = rootex.new_usertype<Component>("Component");
	component["getOwner"] = &Component::getOwner;
	component["getComponentID"] = &Component::getComponentID;
	component["getName"] = &Component::getName;
}

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
    , m_Components(components)
    , m_IsEditorOnly(false)
{
	setName(name);
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

bool Entity::setupEntities()
{
	bool status = true;
	for (auto& component : m_Components)
	{
		status = status & component.second->setupEntities();
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

const String& Entity::getFullName() const
{
	return m_FullName;
}

bool Entity::hasComponent(ComponentID componentID)
{
	return m_Components.find(componentID) != m_Components.end();
}

void Entity::setName(const String& name)
{
	m_Name = name;
	m_FullName = m_Name + " #" + std::to_string(getID());
}

const HashMap<ComponentID, Ref<Component>>& Entity::getAllComponents() const
{
	return m_Components;
}
