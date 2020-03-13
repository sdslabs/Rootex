#include "test_component.h"

Component* TestComponent::Create(const JSON::json& componentData)
{
	TestComponent* component = new TestComponent();
	component->m_TestVariable = componentData["m_TestVariable"];

	return component;
}

Component* TestComponent::CreateDefault()
{
	TestComponent* component = new TestComponent();
	component->m_TestVariable = 0;
	return component;
}

JSON::json TestComponent::getJSON() const
{
	return {};
}
