#include "debug_system.h"

#include "components/debug_component.h"

DebugSystem::DebugSystem()
    : System("DebugSystem", UpdateOrder::None)
{
}

void DebugSystem::update(float deltaMilliseconds)
{
	for (auto& component : s_Components[DebugComponent::s_ID])
	{
		OS::PrintLine("Found 1 DebugComponent. EntityID: " + 
			std::to_string(component->getOwner()->getID()) + 
			". # of components: " + std::to_string(component->getOwner()->getAllComponents().size()));
	}
}
