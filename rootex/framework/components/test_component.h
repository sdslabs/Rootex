#pragma once

#include "component.h"

class TestComponent : public Component
{
	static Component* Create(const LuaVariable& componentData);

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::Test;

	int m_TestVariable;
	ComponentID getComponentID() const override { return s_ID; }
};
