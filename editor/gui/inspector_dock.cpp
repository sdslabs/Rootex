#include "inspector_dock.h"

#include "entity.h"
#include "framework/scene_loader.h"
#include "framework/ecs_factory.h"
#include "editor/editor_system.h"

#include "imgui.h"
#include "ImGuiFileDialog.h"
#include "imgui_stdlib.h"

InspectorDock* InspectorDock::s_Singleton = nullptr;

Variant InspectorDock::openScene(const Event* event)
{
	m_OpenedScene = Extract<Scene*>(event->getData());
	m_OpenedSceneName = m_OpenedScene->getName();
	m_IsNameBeingEdited = false;
	refreshAddNewComponentSelectionCache();
	return true;
}

Variant InspectorDock::closeScene(const Event* event)
{
	m_OpenedScene = nullptr;
	m_OpenedSceneName = "";
	m_ActionScene = nullptr;
	m_IsNameBeingEdited = false;
	return true;
}

void InspectorDock::refreshAddNewComponentSelectionCache()
{
	m_AddNewComponentSelectionCache.clear();
	for (auto& componentDetail : ECSFactory::GetComponentDatabase())
	{
		m_AddNewComponentSelectionCache.push_back({ std::get<ComponentID>(componentDetail), std::get<String>(componentDetail), false });
	}
}

InspectorDock::InspectorDock()
    : m_OpenedScene(nullptr)
{
	BIND_EVENT_MEMBER_FUNCTION("EditorOpenScene", openScene);
	BIND_EVENT_MEMBER_FUNCTION("EditorCloseScene", closeScene);

	if (!s_Singleton)
	{
		s_Singleton = this;
	}
	else
	{
		WARN("Multiple Editor Inspector docks are active");
	}
}

void InspectorDock::drawSceneActions(Scene* scene)
{
	m_ActionScene = scene;
	if (!m_OpenedScene)
	{
		EventManager::GetSingleton()->call("OpenEntity", "EditorOpenScene", m_ActionScene);
	}
	if (m_ActionScene != SceneLoader::GetSingleton()->getRootScene().get())
	{
		if (ImGui::MenuItem("Reload"))
		{
			m_ActionScene->reload();
		}

		if (Entity* entity = m_ActionScene->getEntity())
		{
			if (ImGui::MenuItem("Add Components"))
			{
				m_MenuAction = "Add Components";
			}
			if (ImGui::MenuItem("Remove Components"))
			{
				m_MenuAction = "Remove Components";
			}
			if (ImGui::MenuItem("Reset"))
			{
				PANIC(entity->onAllComponentsAdded() == false, "Could not setup entity: " + entity->getFullName());
			}
			if (ImGui::MenuItem("Remove Entity"))
			{
				m_ActionScene->setEntity(nullptr);
			}
		}
		else
		{
			if (ImGui::MenuItem("Add Entity"))
			{
				m_ActionScene->setEntity(ECSFactory::CreateEmptyEntity(m_ActionScene));
			}	
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Add Empty Scene"))
		{
			m_ActionScene->addChild(Scene::CreateEmpty());
		}

		if (ImGui::MenuItem("Add Empty Scene Entity"))
		{
			m_ActionScene->addChild(Scene::CreateEmptyWithEntity());
		}

		if (ImGui::MenuItem("Add Child Scene From File"))
		{
			igfd::ImGuiFileDialog::Instance()->OpenModal("ChooseChildSceneFile", "Choose Scene File", ".json", "game/assets/");
		}

		if (ImGui::MenuItem("Save Scene to File"))
		{
			igfd::ImGuiFileDialog::Instance()->OpenModal("Save Scene", "Save Scene to File", 0, ".");
		}
		if (igfd::ImGuiFileDialog::Instance()->FileDialog("Save Scene"))
		{
			if (igfd::ImGuiFileDialog::Instance()->IsOk)
			{
				String filePath = OS::GetRootRelativePath(igfd::ImGuiFileDialog::Instance()->GetCurrentPath()).generic_string() + scene->getFullName() + ".scene.json";
				if (!SceneLoader::GetSingleton()->saveSceneAtFile(m_ActionScene, filePath))
				{
					WARN("Could not save selected scene to file: " + filePath);
				}
			}
			igfd::ImGuiFileDialog::Instance()->CloseDialog("Save Scene");
		}

		if (ImGui::Selectable("Copy Scene"))
		{
			if (Ref<Scene> copiedScene = Scene::Create(m_ActionScene->getJSON()))
			{
				m_ActionScene->getParent()->addChild(copiedScene);
			}
			else
			{
				WARN("Could not copy from selected scene");
			}
		}
		if (ImGui::Selectable("Delete Scene"))
		{
			EventManager::GetSingleton()->deferredCall("InspectorUnselectScene", "DeleteScene", m_ActionScene);
			EventManager::GetSingleton()->deferredCall("InspectorDeleteScene", "EditorCloseScene", 0);
		}
	}
}

void InspectorDock::draw(float deltaMilliseconds)
{
	ZoneScoped;
	if (m_InspectorSettings.m_IsActive)
	{
		if (ImGui::Begin("Inspector"))
		{
			if (m_OpenedScene)
			{
				if (m_IsNameBeingEdited)
				{
					if (ImGui::InputText("Scene Name", &m_OpenedSceneName, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
					{
						m_OpenedScene->setName(m_OpenedSceneName);
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
					ImGui::TreeNodeEx(m_OpenedScene->getFullName().c_str(), ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Selected);
					if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
					{
						m_IsNameBeingEdited = true;
					}
					EditorSystem::GetSingleton()->popFont();
				}

				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
				if (ImGui::BeginCombo("##SceneActions", "Select an action"))
				{
					drawSceneActions(m_OpenedScene);
					ImGui::EndCombo();
				}
				if (igfd::ImGuiFileDialog::Instance()->FileDialog("ChooseChildSceneFile"))
				{
					if (igfd::ImGuiFileDialog::Instance()->IsOk)
					{
						FilePath filePath = OS::GetRootRelativePath(igfd::ImGuiFileDialog::Instance()->GetFilePathName());
						m_ActionScene->addChild(Scene::CreateFromFile(filePath.generic_string()));
					}
					igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseChildSceneFile");
				}

				if (!m_MenuAction.empty())
				{
					ImGui::OpenPopup((m_MenuAction + ": " + m_ActionScene->getName()).c_str());
					m_MenuAction.clear();
				}

				ImGui::Separator();

				if (Entity* entity = m_OpenedScene->getEntity())
				{
					EditorSystem::GetSingleton()->pushBoldFont();
					ImGui::Text("Components");
					for (auto& component : entity->getAllComponents())
					{
						ImGui::PushStyleColor(ImGuiCol_Text, EditorSystem::GetSingleton()->getColors().white);
						if (ImGui::TreeNodeEx(component.second->getName(), ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen))
						{
							ImGui::PopStyleColor();
							EditorSystem::GetSingleton()->pushRegularFont();
							component.second->draw();
							EditorSystem::GetSingleton()->popFont();
						}
						else
						{
							ImGui::PopStyleColor();
						}
					}
					EditorSystem::GetSingleton()->popFont();

				}
				if (m_ActionScene)
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
	if (Entity* actionEntity = m_ActionScene->getEntity())
	{
		ImGui::SetNextWindowSize({ ImGui::GetWindowWidth(), ImGui::GetWindowHeight() });
		if (ImGui::BeginPopupModal(("Add Components: " + actionEntity->getName()).c_str(), 0, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("%s", "Choose Components");
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
			if (ImGui::ListBoxHeader("##Choose Components"))
			{
				for (auto&& [componentID, componentName, isComponentSelected] : m_AddNewComponentSelectionCache)
				{
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
					if (!actionEntity->hasComponent(componentID))
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
						if (ECSFactory::AddComponent(actionEntity, ECSFactory::CreateDefaultComponent(componentName)))
						{
							PRINT("Added " + componentName + " to " + actionEntity->getName());
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
}

void InspectorDock::drawRemoveComponentWindow()
{
	if (Entity* actionEntity = m_ActionScene->getEntity())
	{
		ImGui::SetNextWindowSize({ ImGui::GetWindowWidth(), ImGui::GetWindowHeight() });
		if (ImGui::BeginPopupModal(("Remove Components: " + actionEntity->getName()).c_str(), 0, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("%s", "Choose Components");
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
			if (ImGui::ListBoxHeader("##Choose Components"))
			{
				for (auto&& [componentID, componentName, isComponentSelected] : m_AddNewComponentSelectionCache)
				{
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
					if (actionEntity->hasComponent(componentID))
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
						Ref<Component> component = actionEntity->getComponentFromID(componentID);
						if (component)
						{
							if (actionEntity->removeComponent(componentID))
							{
								PRINT("Deleted " + componentName + " from " + actionEntity->getName());
							}
							else
							{
								PRINT("Could not delete component: " + component->getName());
							}
						}
						else
						{
							ERR("Component not found: Possible level file corruption");
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
}
