#pragma once

#include "common/common.h"
#include "script/interpreter.h"

class Component;
typedef unsigned int ComponentID;
typedef unsigned int EntityID;

class Entity
{
	EntityID m_ID;
	HashMap<ComponentID, Ref<Component>> m_Components;

	void addComponent(const Ref<Component>& component);

	Entity(EntityID id);

	bool setupComponents();

	friend class EntityFactory;

public:
	virtual ~Entity() = default;

	void destroy();

	EntityID getID() const;
	template <class ComponentType>
	ComponentType* getComponent();
	const HashMap<ComponentID, Ref<Component>>& getAllComponents() const;
};

template <class ComponentType>
inline ComponentType* Entity::getComponent()
{
	auto findIt = m_Components.find(ComponentType::s_ID);
	if (findIt != m_Components.end())
	{
		Ref<Component> baseTypeComponent = findIt->second;
		return reinterpret_cast<ComponentType*>(baseTypeComponent.get());
	}

	return nullptr;
}
