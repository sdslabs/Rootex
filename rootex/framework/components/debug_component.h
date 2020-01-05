#pragma once

#include "component.h"

class DebugComponent : public Component
{
	static Component* Create(const LuaVariable& componentData);

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::Debug;

	ComponentID getComponentID() const { return s_ID; }
};
