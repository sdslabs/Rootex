#pragma once

#include "component.h"

class TestComponent : public Component
{
	static Component* Create(const LuaVariable& componentData);

	friend class EntityFactory;

public:
	virtual String getName() const override { return "TestComponent"; }
	static const ComponentID s_ID = (ComponentID)ComponentIDs::TestComponent;

	int m_TestVariable;
	ComponentID getComponentID() const override { return s_ID; }
};
