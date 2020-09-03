#include "system.h"

HashMap<ComponentID, Vector<Component*>> System::s_Components;
Vector<System*> System::s_SystemStack;

void System::sort()
{
	std::sort(s_SystemStack.begin(), s_SystemStack.end(), [](System* a, System* b) { return (int)a->getUpdateOrder() < (int)b->getUpdateOrder(); });
}

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
		ERR("Found an unregistered component queued for deregisteration: " + component->getName());
	}
}

System::System(const String& name, const UpdateOrder& order)
    : m_SystemName(name)
    , m_UpdateOrder(order)
{
	s_SystemStack.push_back(this);
	sort();
}

System::~System()
{
	auto& findIt = std::find(s_SystemStack.begin(), s_SystemStack.end(), this);
	if (findIt != s_SystemStack.end())
	{
		s_SystemStack.erase(findIt);
	}
	sort();
}

bool System::initialize(const JSON::json& systemData)
{
	PRINT("On demand initialization skipped for: " + m_SystemName);
	return true;
}

void System::update(float deltaMilliseconds)
{
}

void System::end()
{
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void System::draw()
{
	ImGui::Text("Name: %s", m_SystemName.c_str());
}
#endif // ROOTEX_EDITOR
