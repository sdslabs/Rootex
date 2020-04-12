#pragma once

#include "common/common.h"

#include "entity.h"
#include "component.h"

/// ECS style System interface that allows iterating over components directly.
class System
{
protected:
	static HashMap<ComponentID, Vector<Component*>> s_Components;
	static void RegisterComponent(Component* component);
	static void DeregisterComponent(Component* component);
	
	friend class Entity;
	friend class EntityFactory;

public:
	System() = default;
	System(System&) = delete;
	virtual ~System() = default;

	static const Vector<Component*>& GetComponents(ComponentID ID) { return s_Components[ID]; }
};
