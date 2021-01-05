#pragma once

#include "component.h"

class TestComponent : public Component
{
	static Component* Create(const JSON::json& componentData);

	friend class ECSFactory;

public:
	virtual const char* getName() const override { return "TestComponent"; }
	static const ComponentID s_ID = (ComponentID)ComponentIDs::TestComponent;

	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;
};
