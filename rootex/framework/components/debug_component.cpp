#include "debug_component.h"

#include "entity.h"

Component* DebugComponent::Create(const JSON::json& componentData)
{
	DebugComponent* component = new DebugComponent();
	return component;
}

JSON::json DebugComponent::getJSON() const
{
	return JSON::json::object();
}
