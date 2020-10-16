#include "inspector_dock.h"

#include "entity.h"
#include "framework/components/script_component.h"
#include "entity_factory.h"
#include "framework/component.h"
#include "editor/editor_system.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_stdlib.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

InspectorDock* InspectorDock::s_Singleton = nullptr;

Variant InspectorDock::openEntity(const Event* event)
{
	m_OpenedEntity = Extract(Ref<Entity>, event->getData());
	m_OpenedEntityName = m_OpenedEntity->getName();
	m_IsNameBeingEdited = false;
	refreshAddNewComponentSelectionCache();
	return true;
}

Variant InspectorDock::closeEntity(const Event* event)
{
	m_OpenedEntity = nullptr;
	m_OpenedEntityName = "";
	m_IsNameBeingEdited = false;
	return true;
}

void InspectorDock::refreshAddNewComponentSelectionCache()
{
	m_AddNewComponentSelectionCache.clear();
	for (auto& componentDetail : EntityFactory::GetSingleton()->getComponentDatabase())
	{
		m_AddNewComponentSelectionCache.push_back({ Extract(ComponentID, componentDetail), Extract(String, componentDetail), false });
	}
}

InspectorDock::InspectorDock()
    : m_OpenedEntity(nullptr)
{
	BIND_EVENT_MEMBER_FUNCTION("EditorOpenEntity", openEntity);
	BIND_EVENT_MEMBER_FUNCTION("EditorCloseEntity", closeEntity);

	if (!s_Singleton)
	{
		s_Singleton = this;
	}
	else
	{
		WARN("Multiple Editor Inspector docks are active");
	}
}

void InspectorDock::drawEntityActions(Ref<Entity> actionEntity)
{
	m_ActionEntity = actionEntity;
	if (!m_OpenedEntity)
	{
		EventManager::GetSingleton()->call("OpenEntity", "EditorOpenEntity", m_ActionEntity);
	}
	if (ImGui::Selectable("Add Components"))
	{
		m_MenuAction = "Add Components";
	}
	if (ImGui::Selectable("Remove Components"))
	{
		m_MenuAction = "Remove Components";
	}
	if (ImGui::Selectable("Reset"))
	{
		PANIC(m_ActionEntity->setupComponents() == false, "Could not setup entity: " + m_ActionEntity->getFullName());
	}
	if (ImGui::Selectable("Save Entity as class"))
	{
		if (!EntityFactory::GetSingleton()->saveEntityAsClass(m_ActionEntity))
		{
			WARN("Could not create class from selected entity");
		}
	}
	if (ImGui::Selectable("Copy Entity"))
	{
		if (!EntityFactory::GetSingleton()->copyEntity(m_ActionEntity))
		{
			WARN("Could not copy from selected entity");
		}
	}
	ImGui::Separator();
	if (ImGui::Selectable("Delete Entity"))
	{
		if (m_ActionEntity->getID() != ROOT_ENTITY_ID)
		{
			EventManager::GetSingleton()->deferredCall("EditorDeleteEntity", "DeleteEntity", m_ActionEntity);
		}
		else
		{
			WARN("Cannot delete Root Entity");
		}
	}
}

void InspectorDock::draw(float deltaMilliseconds)
{
	if (m_InspectorSettings.m_IsActive)	
	{
		if (ImGui::Begin("Inspector"))
		{
			if (m_OpenedEntity)
			{
				if (m_IsNameBeingEdited)
				{
					if (ImGui::InputText("Entity Name", &m_OpenedEntityName, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
					{
						m_OpenedEntity->setName(m_OpenedEntityName);
						m_IsNameBeingEdited = false;
					}
					if (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						m_IsNameBeingEdited = false;
					}
				}
				else
				{
					EditorSystem::GetSingleton()->pushBoldFont();
					ImGui::TreeNodeEx(m_OpenedEntity->getFullName().c_str(), ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Selected);
					if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
					{
						m_IsNameBeingEdited = true;
					}
					EditorSystem::GetSingleton()->popFont();
				}

				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
				if (ImGui::BeginCombo("##Entity Actions", "Select an action"))
				{
					drawEntityActions(m_OpenedEntity);
					ImGui::EndCombo();
				}

				if (!m_MenuAction.empty())
				{
					ImGui::OpenPopup((m_MenuAction + ": " + m_ActionEntity->getName()).c_str());
					m_MenuAction.clear();
				}

				ImGui::Separator();

				m_OpenedEntity->draw();

				ImGui::Separator();

				EditorSystem::GetSingleton()->pushBoldFont();
				ImGui::Text("Components");
				for (auto& component : m_OpenedEntity->getAllComponents())
				{
					if (ImGui::TreeNodeEx(component.second->getName().c_str(), ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen))
					{
						EditorSystem::GetSingleton()->pushRegularFont();
						component.second->draw();
						EditorSystem::GetSingleton()->popFont();
					}
				}
				EditorSystem::GetSingleton()->popFont();

				if (m_ActionEntity)
				{
					drawAddComponentWindow();
					drawRemoveComponentWindow();
				}
			}
		}
		ImGui::End();
	}
}

void InspectorDock::drawAddComponentWindow()
{
	ImGui::SetNextWindowSize({ ImGui::GetWindowWidth(), ImGui::GetWindowHeight() });
	if (ImGui::BeginPopupModal(("Add Components: " + m_ActionEntity->getName()).c_str(), 0, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("%s", "Choose Components");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
		if (ImGui::ListBoxHeader("##Choose Components"))
		{
			for (auto&& [componentID, componentName, isComponentSelected] : m_AddNewComponentSelectionCache)
			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
				if (!m_ActionEntity->hasComponent(componentID))
				{
					ImGui::Checkbox(componentName.c_str(), &isComponentSelected);
				}
			}
			ImGui::ListBoxFooter();
		}

		ImGui::Text("%s", "Components");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
		if (ImGui::ListBoxHeader("##Selected Components"))
		{
			for (auto&& [componentID, componentName, isComponentSelected] : m_AddNewComponentSelectionCache)
			{
				if (isComponentSelected)
				{
					ImGui::Text(componentName.c_str());
				}
			}
			ImGui::ListBoxFooter();
		}

		ImGui::TextWrapped("Entities will be automatically reset to make new components function properly.");

		if (ImGui::Button("Add"))
		{
			for (auto&& [componentID, componentName, isComponentSelected] : m_AddNewComponentSelectionCache)
			{
				if (isComponentSelected)
				{
					Ref<Component> component = EntityFactory::GetSingleton()->createDefaultComponent(componentName);
					EntityFactory::GetSingleton()->addComponent(m_ActionEntity, component);
					PRINT("Added " + componentName + " to " + m_ActionEntity->getName());
				}
			}
			refreshAddNewComponentSelectionCache();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			refreshAddNewComponentSelectionCache();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void InspectorDock::drawRemoveComponentWindow()
{
	ImGui::SetNextWindowSize({ ImGui::GetWindowWidth(), ImGui::GetWindowHeight() });
	if (ImGui::BeginPopupModal(("Remove Components: " + m_ActionEntity->getName()).c_str(), 0, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("%s", "Choose Components");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
		if (ImGui::ListBoxHeader("##Choose Components"))
		{
			for (auto&& [componentID, componentName, isComponentSelected] : m_AddNewComponentSelectionCache)
			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
				if (m_ActionEntity->hasComponent(componentID))
				{
					ImGui::Checkbox(componentName.c_str(), &isComponentSelected);
				}
			}
			ImGui::ListBoxFooter();
		}

		ImGui::Text("%s", "Components");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
		if (ImGui::ListBoxHeader("##Selected Components"))
		{
			for (auto&& [componentID, componentName, isComponentSelected] : m_AddNewComponentSelectionCache)
			{
				if (isComponentSelected)
				{
					ImGui::Text(componentName.c_str());
				}
			}
			ImGui::ListBoxFooter();
		}

		ImGui::TextWrapped("Entities will be automatically reset to make old components function properly.");

		if (ImGui::Button("Remove"))
		{
			for (auto&& [componentID, componentName, isComponentSelected] : m_AddNewComponentSelectionCache)
			{
				if (isComponentSelected)
				{
					Ref<Component> component = m_ActionEntity->getComponentFromID(componentID);
					if (component)
					{
						m_ActionEntity->removeComponent(component);
						PRINT("Deleted " + componentName + " from " + m_ActionEntity->getName());
					}
					else
					{
						ERR("Component not found: Possible level file corruption");
					}
				}
				m_ActionEntity->setupComponents();
			}
			refreshAddNewComponentSelectionCache();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			refreshAddNewComponentSelectionCache();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}
