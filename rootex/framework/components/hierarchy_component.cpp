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
    , m_Parent(nullptr)
{
}

bool HierarchyComponent::addChild(Ref<Entity> child)
{
	if (auto&& findIt = std::find(m_ChildrenIDs.begin(), m_ChildrenIDs.end(), child->getID()) == m_ChildrenIDs.end())
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
	auto& findIt = std::find(m_ChildrenIDs.begin(), m_ChildrenIDs.end(), node->getID());
	if (findIt != m_ChildrenIDs.end())
	{
		Ref<HierarchyComponent> hc = node->getComponent<HierarchyComponent>();
		hc->m_Parent = nullptr;
		hc->m_ParentID = INVALID_ID;

		auto&& findItPtr = std::find(m_Children.begin(), m_Children.end(), hc.get());

		m_Children.erase(findItPtr);
		m_ChildrenIDs.erase(findIt);

		return true;
	}
	return false;
}

bool HierarchyComponent::snatchChild(Ref<Entity> node)
{
	node->getComponent<HierarchyComponent>()->getParent()->removeChild(node);
	bool status = addChild(node);

	return status;
}

void HierarchyComponent::clear()
{
	m_Parent = nullptr;
	m_ParentID = INVALID_ID;
	m_Children.clear();
	m_ChildrenIDs.clear();
}

void HierarchyComponent::onRemove()
{
	Vector<HierarchyComponent*> backup = m_Children;
	for (auto& child : backup)
	{
		if (m_Parent)
		{
			m_Parent->snatchChild(child->getOwner());
		}
	}
	if (m_Parent)
	{
		m_Parent->removeChild(m_Owner);
	}
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
	String parentName = m_Parent ? m_Parent->getOwner()->getFullName() : "None";
	if (m_Parent)
	{
		if (ImGui::BeginCombo("##Parent", parentName.c_str(), ImGuiComboFlags_HeightLarge))
		{
			for (auto&& [entityID, entity] : EntityFactory::GetSingleton()->getEntities())
			{
				if (ImGui::Selectable(entity->getFullName().c_str()))
				{
					bool status = entity->getComponent<HierarchyComponent>()->snatchChild(getOwner());
					PANIC(status == false, "Could not set parent as " + entity->getFullName() + " for entity " + getOwner()->getFullName());
				}
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		if (ImGui::Button("Parent")) // Not available for Root (Root.m_Parent is nullptr)
		{
			EventManager::GetSingleton()->call("OpenChildEntity", "EditorOpenEntity", m_Parent->getOwner());
		}
	}

	if (ImGui::ListBoxHeader("Children", m_Children.size() ? m_Children.size() : 1))
	{
		for (auto&& child : m_Children)
		{
			if (ImGui::Selectable(child->getOwner()->getFullName().c_str()))
			{
				EventManager::GetSingleton()->call("OpenChildEntity", "EditorOpenEntity", child->getOwner());
			}
		}
		ImGui::ListBoxFooter();
	}
}
#endif // ROOTEX_EDITOR
