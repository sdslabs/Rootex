#include "debug_system.h"

#include "components/debug_component.h"

DebugSystem::DebugSystem()
    : System("DebugSystem", UpdateOrder::Async, false)
{
}

void DebugSystem::reportComponents()
{
	for (auto& component : s_Components[DebugComponent::s_ID])
	{
		OS::PrintLine("Found 1 DebugComponent in " + 
			component->getOwner()->getScene()->getFullName() + 
			". # of components in scene's entity: " + std::to_string(component->getOwner()->getAllComponents().size()));
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
