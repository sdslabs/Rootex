#pragma once

#include "component.h"

class DebugComponent : public Component
{
	static Component* Create(const JSON::json& componentData);

	friend class ECSFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::DebugComponent;

	virtual const char* getName() const override { return "DebugComponent"; }
	ComponentID getComponentID() const { return s_ID; }
	virtual JSON::json getJSON() const override;
};
