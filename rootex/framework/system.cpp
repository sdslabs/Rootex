#include "system.h"

HashMap<ComponentID, Vector<Component*>> System::s_Components;

void System::RegisterComponent(Component* component)
{
	auto findIt = s_Components.find(component->getComponentID());
	if (findIt != s_Components.end())
	{
		s_Components[component->getComponentID()].push_back(component);
	}
	else
	{
		s_Components[component->getComponentID()] = { component };
	}
}

void System::DeregisterComponent(Component* component)
{
	Vector<Component*>& components = s_Components[component->getComponentID()];

	auto findIt = std::find(components.begin(), components.end(), component);
	if (findIt != components.end())
	{
		components.erase(findIt);
	}
	else
	{
		ERR("Tried to erase a non-existing component on deletion: " + std::to_string(component->getComponentID()));
	}
}
