#pragma once

#include "common/common.h"
#include "script/interpreter.h"

#include "components/component_ids.h"

typedef unsigned int ComponentID;

class Entity;

class Component
{
protected:
	Ref<Entity> m_Owner;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::Component;

	Component();
	virtual ~Component();
	
	virtual bool setup();

	Ref<Entity> getOwner() const;
	virtual ComponentID getComponentID() const = 0;

#ifdef ROOTEX_EDITOR
	virtual void draw();
#endif

	virtual String getName() const = 0;
	void setOwner(Ref<Entity>& newOwner) { m_Owner = newOwner; }
};
