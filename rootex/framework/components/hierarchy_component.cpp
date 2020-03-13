#include "hierarchy_component.h"
#include "event_manager.h"

Component* HierarchyComponent::Create(const LuaVariable& componentData)
{
	HierarchyComponent* component = new HierarchyComponent();
	return component;
}

bool HierarchyComponent::addChild(Ref<Entity> child)
{
	m_Children.push_back(child);
	child->getComponent<HierarchyComponent>()->m_Parent = this->m_Owner;
	return true;
}

bool HierarchyComponent::removeChild(Ref<Entity> node)
{
	auto& findIt = std::find(m_Children.begin(), m_Children.end(), node);
	if (findIt != m_Children.end())
	{
		(*findIt)->getComponent<HierarchyComponent>()->m_Parent = nullptr;
		m_Children.erase(findIt);
	}
	return true;
}

JSON::json HierarchyComponent::getJSON() const
{
	JSON::json j;

	if (!m_Parent)
	{
		j["parent"]["ID"] = 1;
		j["parent"]["name"] = "Root";	
	}
	else
	{
		j["parent"]["ID"] = m_Parent->getID();
		j["parent"]["name"] = m_Parent->getName();
	}

	JSON::json& jc = j["children"];
	for (auto&& child : m_Children)
	{
		jc[child->getID()] = child->getName();
	}

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void HierarchyComponent::draw()
{
	ImGui::Columns(2);

	if (m_Parent)
	{
		ImGui::Text("Parent");
		ImGui::NextColumn();
		ImGui::Text(m_Parent->getName().c_str());
		ImGui::SameLine();
		if (ImGui::Button("Go"))
		{
			EventManager::GetSingleton()->call("OpenChildEntity", "EditorInspectorOpenEntity", m_Parent);
		}
		ImGui::NextColumn();
	}
	else
	{
		ImGui::Text("Parent");
		ImGui::NextColumn();
		ImGui::Text("None");
		ImGui::NextColumn();
	}

	ImGui::Separator();

	ImGui::Text("Children");
	ImGui::NextColumn();
	if (m_Children.size() == 0)
	{
		ImGui::Text("None");
	}
	for (auto& child : m_Children)
	{
		ImGui::Text(child->getName().c_str());
		ImGui::SameLine();
		if (ImGui::Button(("Go##" + child->getName()).c_str()))
		{
			EventManager::GetSingleton()->call("OpenChildEntity", "EditorInspectorOpenEntity", child);
		}
	}

	ImGui::Columns(1);
}
#endif // ROOTEX_EDITOR
