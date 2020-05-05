#pragma once

#include "common/common.h"
#include "script/interpreter.h"
#include "components/component_ids.h"

typedef unsigned int ComponentID;

/// An ECS style interface of a collection of data that helps implement a behaviour. Also allows operations on that data.
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
	
	/// Perform setting up operations which are possible only after all planned components are added to the owning entity.
	virtual bool setup();
	/// Perform setting up operations which are possible only after all entities have been set up.
	virtual bool setupEntities();
	virtual void onRemove();

	Ref<Entity> getOwner() const;
	virtual ComponentID getComponentID() const = 0;
	virtual String getName() const = 0;
	/// Get JSON representation of the component data needed to re-construct component from memory.
	virtual JSON::json getJSON() const;

#ifdef ROOTEX_EDITOR
	/// Expose the component data in the InspectorDock.
	virtual void draw();
#endif
};
