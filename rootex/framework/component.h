#pragma once

#include "common/common.h"
#include "script/interpreter.h"
#include "components/component_ids.h"

typedef unsigned int ComponentID;

class Component
{
	void setOwner(Ref<Entity>& newOwner) { m_Owner = newOwner; }
	friend class EntityFactory;

protected:
	Ref<Entity> m_Owner;
	
public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::Component;

	Component();
	virtual ~Component();
	
	virtual bool setup();
	virtual void onRemove();

	Ref<Entity> getOwner() const;
	virtual ComponentID getComponentID() const = 0;
	virtual String getName() const = 0;
	virtual JSON::json getJSON() const;

#ifdef ROOTEX_EDITOR
	virtual void draw();
#endif
};
