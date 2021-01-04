#include "test_component.h"

Component* TestComponent::Create(const JSON::json& componentData)
{
	TestComponent* component = new TestComponent();
	return component;
}

JSON::json TestComponent::getJSON() const
{
	return JSON::json::object();
}
