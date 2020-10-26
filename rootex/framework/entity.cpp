#include "entity.h"

#include "event_manager.h"
#include "framework/component.h"
#include "framework/system.h"

void Entity::RegisterAPI(sol::table& rootex)
{
	sol::usertype<Entity> entity = rootex.new_usertype<Entity>("Entity");
	entity["removeComponent"] = &Entity::removeComponent;
	entity["destroy"] = &Entity::destroy;
	entity["hasComponent"] = &Entity::hasComponent;
	entity["getScene"] = &Entity::getScene;

	sol::usertype<Component> component = rootex.new_usertype<Component>("Component");
	component["getOwner"] = &Component::getOwner;
	component["getComponentID"] = &Component::getComponentID;
	component["getName"] = &Component::getName;
}

Entity::Entity(Scene* scene)
    : m_Scene(scene)
{
}

Entity::~Entity()
{
	destroy();
}

JSON::json Entity::getJSON() const
{
	JSON::json j;
	j["components"] = {};
	for (auto&& [componentID, component] : m_Components)
	{
		j["components"][component->getName()] = component->getJSON();
	}

	return j;
}

bool Entity::onAllComponentsAdded()
{
	bool status = true;
	for (auto& component : m_Components)
	{
		status = status & component.second->setup();
	}
	return status;
}

bool Entity::onAllEntitiesAdded()
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

bool Entity::removeComponent(ComponentID toRemoveComponentID)
{
	Ref<Component> toRemoveComponent = getComponentFromID(toRemoveComponentID);
	for (auto& [componentID, component] : m_Components)
	{
		for (auto& dependency : component->getDependencies())
		{
			if (dependency->getID() == toRemoveComponentID)
			{
				WARN("Entity has other components depending on the to-be-removed component " + toRemoveComponent->getName());
				WARN("Component deletion denied");
				return false;
			}
		}
	}

	toRemoveComponent->onRemove();
	System::DeregisterComponent(toRemoveComponent.get());
	m_Components.erase(toRemoveComponent->getComponentID());

	return true;
}

bool Entity::hasComponent(ComponentID componentID)
{
	return m_Components.find(componentID) != m_Components.end();
}

const HashMap<ComponentID, Ref<Component>>& Entity::getAllComponents() const
{
	return m_Components;
}
