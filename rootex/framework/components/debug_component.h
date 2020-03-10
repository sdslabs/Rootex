#pragma once

#include "component.h"

class DebugComponent : public Component
{
	static Component* Create(const LuaVariable& componentData);
	static Component* CreateDefault();

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::DebugComponent;

	virtual String getName() const override { return "DebugComponent"; }
	ComponentID getComponentID() const { return s_ID; }
};
