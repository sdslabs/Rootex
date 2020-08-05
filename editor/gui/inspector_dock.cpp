#include "inspector_dock.h"

#include "entity.h"
#include "entity_factory.h"
#include "framework/component.h"
#include "editor/editor.h"

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

	if (!s_Singleton)
	{
		s_Singleton = this;
	}
	else
	{
		WARN("Multiple Editor Inspector docks are active");
	}
}

void InspectorDock::draw()
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
					ImGui::TreeNodeEx(m_OpenedEntity->getFullName().c_str(), ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Selected);
					if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
					{
						m_IsNameBeingEdited = true;
					}
				}

				String menuAction;

				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
				if (ImGui::BeginCombo("##Entity Actions", "Select an action"))
				{
					if (ImGui::Selectable("Add Components"))
					{
						menuAction = "Add Components";
					}
					if (ImGui::Selectable("Remove Components"))
					{
						menuAction = "Remove Components";
					}
					if (ImGui::Selectable("Reset"))
					{
						PANIC(m_OpenedEntity->setupComponents() == false, "Could not setup entity: " + m_OpenedEntity->getFullName());
					}
					if (ImGui::Selectable("Save Entity as class"))
					{
						if (!EntityFactory::GetSingleton()->saveEntityAsClass(m_OpenedEntity))
						{
							WARN("Could not create class from selected entity");
						}
					}
					ImGui::Separator();
					if (ImGui::Selectable("Delete Entity"))
					{
						if (m_OpenedEntity->getID() != ROOT_ENTITY_ID)
						{
							EventManager::GetSingleton()->deferredCall("EditorDeleteEntity", "DeleteEntity", m_OpenedEntity);
						}
						else
						{
							WARN("Cannot delete Root Entity");
						}
					}
					ImGui::EndCombo();
				}

				if (!menuAction.empty())
				{
					ImGui::OpenPopup((menuAction + ": " + m_OpenedEntity->getName()).c_str());
				}

				ImGui::Separator();

				ImGui::Text("Components");
				for (auto& component : m_OpenedEntity->getAllComponents())
				{
					if (ImGui::TreeNodeEx(component.second->getName().c_str(), ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen))
					{
						component.second->draw();
					}
				}

				drawAddComponentWindow();
				drawRemoveComponentWindow();
			}
		}
		ImGui::End();
	}
}

void InspectorDock::drawAddComponentWindow()
{
	ImGui::SetNextWindowSize({ ImGui::GetWindowWidth(), ImGui::GetWindowHeight() });
	if (ImGui::BeginPopupModal(("Add Components: " + m_OpenedEntity->getName()).c_str(), 0, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("%s", "Choose Components");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
		if (ImGui::ListBoxHeader("##Choose Components"))
		{
			for (auto&& [componentID, componentName, isComponentSelected] : m_AddNewComponentSelectionCache)
			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
				if (!m_OpenedEntity->hasComponent(componentID))
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
					EntityFactory::GetSingleton()->addComponent(m_OpenedEntity, component);
					PRINT("Added " + componentName + " to " + m_OpenedEntity->getName());
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
	if (ImGui::BeginPopupModal(("Remove Components: " + m_OpenedEntity->getName()).c_str(), 0, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("%s", "Choose Components");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
		if (ImGui::ListBoxHeader("##Choose Components"))
		{
			for (auto&& [componentID, componentName, isComponentSelected] : m_AddNewComponentSelectionCache)
			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
				if (m_OpenedEntity->hasComponent(componentID))
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
					Ref<Component> component = m_OpenedEntity->getComponentFromID(componentID);
					if (component)
					{
						m_OpenedEntity->removeComponent(component);
						PRINT("Deleted " + componentName + " from " + m_OpenedEntity->getName());
					}
				}
				m_OpenedEntity->setupComponents();
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
