#pragma once

#include "common/common.h"

#include "entity.h"
#include "component.h"

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
	virtual ~System() = default;

	virtual void update(float deltaMilliseconds) = 0;
};
