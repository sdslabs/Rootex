#pragma once

#include "common/common.h"

#ifndef SWIG
#include "script/interpreter.h"
#endif

#include "components/component_ids.h"

typedef unsigned int ComponentID;

class Entity;

class Component
{
protected:
	Ref<Entity> m_Owner;

public:
	Component();
	virtual ~Component();
	
	virtual bool setup();

	Entity* getOwner() const;
	virtual ComponentID getComponentID() const = 0;
	
	void setOwner(Ref<Entity> newOwner) { m_Owner = newOwner; }
};
