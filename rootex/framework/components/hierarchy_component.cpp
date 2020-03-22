#include "hierarchy_component.h"
#include "entity_factory.h"
#include "event_manager.h"

Component* HierarchyComponent::Create(const JSON::json& componentData)
{
	PANIC(componentData["parent"] == INVALID_ID, "Found a invalid ID while constructing HierarchyComponent. Only the Root is allowed to use it.");

	HierarchyComponent* component = new HierarchyComponent(componentData["parent"], componentData["children"]);
	return component;
}

Component* HierarchyComponent::CreateDefault()
{
	HierarchyComponent* component = new HierarchyComponent(ROOT_ENTITY_ID, {});
	return component;
}

HierarchyComponent::HierarchyComponent(EntityID parentID, const Vector<EntityID>& childrenIDs)
    : m_ParentID(parentID)
    , m_ChildrenIDs(childrenIDs)
{
	if (parentID == INVALID_ID)
	{
		return;
	}

	Ref<Entity> parent = EntityFactory::GetSingleton()->findEntity(parentID);
	if (parent)
	{
		m_Parent = parent;
	}
	else
	{
		WARN("Tried searching for a parent that is not yet constructed. Reset hierarchies to identify correct parent");
	}

	for (auto&& childID : childrenIDs)
	{
		Ref<Entity> child = EntityFactory::GetSingleton()->findEntity(parentID);
		if (child)
		{
			m_Children.push_back(child);
		}
		else
		{
			WARN("Tried searching for a child that is not yet constructed. Reset hierarchies to identify correct child");
		}
	}
}

bool HierarchyComponent::addChild(Ref<Entity> child)
{
	m_Children.push_back(child);
	m_ChildrenIDs.push_back(child->getID());
	child->getComponent<HierarchyComponent>()->m_Parent = this->m_Owner;
	child->getComponent<HierarchyComponent>()->m_ParentID = this->m_Owner->getID();
	return true;
}

bool HierarchyComponent::removeChild(Ref<Entity> node)
{
	auto& findIt = std::find(m_Children.begin(), m_Children.end(), node);
	if (findIt != m_Children.end())
	{
		(*findIt)->getComponent<HierarchyComponent>()->m_Parent = nullptr;
		(*findIt)->getComponent<HierarchyComponent>()->m_ParentID = INVALID_ID;
		
		auto&& findItID = std::find(m_ChildrenIDs.begin(), m_ChildrenIDs.end(), (*findIt)->getID());

		m_Children.erase(findIt);
		m_ChildrenIDs.erase(findItID);
	}
	return true;
}

JSON::json HierarchyComponent::getJSON() const
{
	JSON::json j;

	if (!m_Parent)
	{
		j["parent"] = 0;
	}
	else
	{
		j["parent"] = m_Parent->getID();
	}

	JSON::json& jc = j["children"];
	jc = m_ChildrenIDs;
	
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
			EventManager::GetSingleton()->call("OpenChildEntity", "EditorOpenEntity", m_Parent);
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
		if (ImGui::Button(("Go##" + std::to_string(child->getID())).c_str()))
		{
			EventManager::GetSingleton()->call("OpenChildEntity", "EditorOpenEntity", child);
		}
	}

	ImGui::Columns(1);
}
#endif // ROOTEX_EDITOR
