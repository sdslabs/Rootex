#include "debug_component.h"

#include "entity.h"

Component* DebugComponent::Create(const LuaVariable& componentData)
{
	DebugComponent* component = new DebugComponent();
	return component;
}
