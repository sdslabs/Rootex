#include "inspector_dock.h"

#include "entity.h"
#include "entity_factory.h"
#include "framework/component.h"
#include "editor/editor.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

Variant InspectorDock::openEntity(const Event* event)
{
	m_OpenedEntity = Extract(Ref<Entity>, event->getData());
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
	BIND_EVENT_MEMBER_FUNCTION("EditorInspectorOpenEntity", openEntity);
}

void InspectorDock::draw()
{
	if (m_InspectorSettings.m_IsActive)
	{
		if (ImGui::Begin("Inspector"))
		{
			if (m_OpenedEntity)
			{
				ImGui::Text("Entity: %s", m_OpenedEntity->getName().c_str());
				ImGui::PushStyleColor(ImGuiCol_Button, Editor::GetSingleton()->getColors().m_Success);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Editor::GetSingleton()->getColors().m_Accent);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, Editor::GetSingleton()->getColors().m_Success);
				if (ImGui::Button("Add Components"))
				{
					ImGui::OpenPopup(("Add Components: " + m_OpenedEntity->getName()).c_str());
				}
				ImGui::PopStyleColor(3);
				ImGui::PushStyleColor(ImGuiCol_Button, Editor::GetSingleton()->getColors().m_Failure);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Editor::GetSingleton()->getColors().m_FailAccent);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, Editor::GetSingleton()->getColors().m_Failure);
				ImGui::SameLine();
				if (ImGui::Button("Remove Components"))
				{
					ImGui::OpenPopup(("Remove Components: " + m_OpenedEntity->getName()).c_str());
				}
				ImGui::PopStyleColor(3);
				ImGui::SameLine();
				if (ImGui::Button("Reset"))
				{
					m_OpenedEntity->setupComponents();
				}
				ImGui::Separator();
				ImGui::Text("Components");
				for (auto& component : m_OpenedEntity->getAllComponents())
				{
					if (ImGui::TreeNodeEx(component.second->getName().c_str(), ImGuiTreeNodeFlags_CollapsingHeader))
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

		ImGui::PushStyleColor(ImGuiCol_Text, Editor::GetSingleton()->getColors().m_Warning);
		ImGui::TextWrapped("Entities need to be reset to make some new components function properly.");
		ImGui::PopStyleColor(1);

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

		if (ImGui::Button("Remove"))
		{
			for (auto&& [componentID, componentName, isComponentSelected] : m_AddNewComponentSelectionCache)
			{
				if (isComponentSelected)
				{
					Ref<Component> component = m_OpenedEntity->getComponent(componentID);
					if (component)
					{
						m_OpenedEntity->removeComponent(component);
						PRINT("Deleted " + componentName + " from " + m_OpenedEntity->getName());
					}
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
