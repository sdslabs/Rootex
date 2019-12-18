#pragma once

#include "common/common.h"
#include "script/interpreter.h"

#include "components/component_ids.h"

typedef unsigned int ComponentID;

class Entity;

class Component
{
	void setOwner(Ref<Entity> newOwner) { m_Owner = newOwner; }
	
	friend class EntityFactory;

protected:
	Ref<Entity> m_Owner;

public:
	Component();
	virtual ~Component();
	
	Entity* getOwner() const;
	virtual ComponentID getComponentID() const = 0;
};
