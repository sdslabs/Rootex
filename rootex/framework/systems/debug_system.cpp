#include "debug_system.h"

#include "components/debug_component.h"

DebugSystem::DebugSystem()
    : System("DebugSystem", UpdateOrder::None, false)
{
}

void DebugSystem::reportComponents()
{
	for (auto& component : s_Components[DebugComponent::s_ID])
	{
		OS::PrintLine("Found 1 DebugComponent. EntityID: " + 
			std::to_string(component->getOwner()->getID()) + 
			". # of components: " + std::to_string(component->getOwner()->getAllComponents().size()));
	}
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void DebugSystem::draw()
{
	System::draw();

	if (ImGui::Button("Check Components"))
	{
		reportComponents();
	}
}
#endif // ROOTEX_EDITOR
