#include "system.h"

HashMap<ComponentID, Vector<Component*>> System::s_Components;
Vector<System*> System::s_SystemStack;
Vector<System*> System::s_GameplayStack;

void System::UpdateOrderSort()
{
	std::sort(s_SystemStack.begin(), s_SystemStack.end(), [](System* a, System* b) { return (int)a->getUpdateOrder() < (int)b->getUpdateOrder(); });
}

void System::CreationOrderSort()
{
	std::sort(s_SystemStack.begin(), s_SystemStack.end(), [](System* a, System* b) { return a->getCreationOrder() < b->getCreationOrder(); });
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

System::System(const String& name, const UpdateOrder& order, bool isGameplay)
    : m_SystemName(name)
    , m_UpdateOrder(order)
{
	s_SystemStack.push_back(this);
	UpdateOrderSort();
	m_CreationOrder = s_SystemStack.size();
	setGameplay(isGameplay);
}

System::~System()
{
	auto& findIt = std::find(s_SystemStack.begin(), s_SystemStack.end(), this);
	if (findIt != s_SystemStack.end())
	{
		s_SystemStack.erase(findIt);
	}
	setGameplay(false);
	UpdateOrderSort();
}

bool System::initialize(const JSON::json& systemData)
{
	PRINT("On demand initialization skipped for: " + m_SystemName);
	return true;
}

void System::begin()
{
}

void System::update(float deltaMilliseconds)
{
}

void System::end()
{
}

void System::setGameplay(bool enabled)
{
	m_IsGameplaySystem = enabled;

	if (m_IsGameplaySystem)
	{
		if (std::find(s_GameplayStack.begin(), s_GameplayStack.end(), this) == s_GameplayStack.end())
		{
			s_GameplayStack.push_back(this);
		}
		m_IsGameplaySystem = true;
	}
	else
	{
		auto& findIt = std::find(s_GameplayStack.begin(), s_GameplayStack.end(), this);
		if (findIt != s_GameplayStack.end())
		{
			s_GameplayStack.erase(findIt);
		}
		m_IsGameplaySystem = false;
	}
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void System::draw()
{
	ImGui::Columns(2);

	ImGui::Text("Name");
	ImGui::NextColumn();
	ImGui::Text("%s", m_SystemName.c_str());
	ImGui::NextColumn();

	ImGui::Text("Gameplay");
	ImGui::NextColumn();
	ImGui::Text("%s", m_IsGameplaySystem ? "True" : "False");
	ImGui::NextColumn();

	ImGui::Text("Update order");
	ImGui::NextColumn();
	static const char* updateOrders[] = {
		"Input",
		"Update",
		"Render",
		"UI",
		"RenderUI",
		"Editor",
		"None"
	};
	ImGui::Combo((String("##Update Order") + m_SystemName).c_str(), (int*)&m_UpdateOrder, updateOrders, 6);
	ImGui::NextColumn();

	ImGui::Columns(1);
}
#endif // ROOTEX_EDITOR
