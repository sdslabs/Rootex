#include "test_component.h"

Component* TestComponent::Create(const LuaVariable& componentData)
{
	TestComponent* component = new TestComponent();
	component->m_TestVariable = componentData["m_TestVariable"];

	return component;
}
