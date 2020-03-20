#include "hierarchy_component.h"
#include "entity_factory.h"
#include "event_manager.h"

Component* HierarchyComponent::Create(const JSON::json& componentData)
{
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
		m_Parent = nullptr;
		return;
	}

	Ref<Entity> parent = EntityFactory::GetSingleton()->findEntity(parentID);
	if (parent)
	{
		m_Parent = parent->getComponent<HierarchyComponent>().get();
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
			m_Children.push_back(child->getComponent<HierarchyComponent>().get());
		}
		else
		{
			WARN("Tried searching for a child that is not yet constructed. Reset hierarchies to identify correct child");
		}
	}
}

bool HierarchyComponent::addChild(Ref<Entity> child)
{
	if (auto&& findIt = std::find(m_Children.begin(), m_Children.end(), child->getComponent<HierarchyComponent>().get()) == m_Children.end())
	{
		m_Children.push_back(child->getComponent<HierarchyComponent>().get());
		m_ChildrenIDs.push_back(child->getID());
		child->getComponent<HierarchyComponent>()->m_Parent = this;
		child->getComponent<HierarchyComponent>()->m_ParentID = this->m_Owner->getID();
		return true;
	}
	return false;
}

bool HierarchyComponent::removeChild(Ref<Entity> node)
{
	auto& findIt = std::find(m_Children.begin(), m_Children.end(), node->getComponent<HierarchyComponent>().get());
	if (findIt != m_Children.end())
	{
		(*findIt)->m_Parent = nullptr;
		(*findIt)->m_ParentID = INVALID_ID;

		auto&& findItID = std::find(m_ChildrenIDs.begin(), m_ChildrenIDs.end(), (*findIt)->getOwner()->getID());

		m_Children.erase(findIt);
		m_ChildrenIDs.erase(findItID);
	}
	return true;
}

void HierarchyComponent::clear()
{
	m_Parent = nullptr;
	m_ParentID = INVALID_ID;
	m_Children.clear();
	m_ChildrenIDs.clear();
}

JSON::json HierarchyComponent::getJSON() const
{
	JSON::json j;

	if (m_Parent)
	{
		j["parent"] = m_Parent->getOwner()->getID();
		j["children"] = m_ChildrenIDs;
	}
	else
	{
		j["parent"] = INVALID_ID;
		j["children"] = m_ChildrenIDs;
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
		ImGui::Text(m_Parent->getOwner()->getName().c_str());
		ImGui::SameLine();
		if (ImGui::Button("Go"))
		{
			EventManager::GetSingleton()->call("OpenChildEntity", "EditorOpenEntity", m_Parent->getOwner());
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
		ImGui::Text(child->getOwner()->getName().c_str());
		ImGui::SameLine();
		if (ImGui::Button(("Go##" + std::to_string(child->getOwner()->getID())).c_str()))
		{
			EventManager::GetSingleton()->call("OpenChildEntity", "EditorOpenEntity", child->getOwner());
		}
	}

	ImGui::Columns(1);
}
#endif // ROOTEX_EDITOR
