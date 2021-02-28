#include "inspector_dock.h"

#include "entity.h"
#include "framework/scene_loader.h"
#include "framework/component.h"
#include "script/script.h"
#include "editor/editor_system.h"
#include "utility/imgui_helpers.h"

#include "imgui.h"
#include "imgui_stdlib.h"

InspectorDock* InspectorDock::s_Singleton = nullptr;

Variant InspectorDock::openScene(const Event* event)
{
	m_OpenedScene = Extract<Scene*>(event->getData());
	m_OpenedSceneName = m_OpenedScene->getName();
	m_IsNameBeingEdited = false;
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
	if (m_ActionScene != SceneLoader::GetSingleton()->getRootScene())
	{
		if (ImGui::MenuItem("Reload"))
		{
			m_ActionScene->reload();
		}

		if (Entity* entity = m_ActionScene->getEntity())
		{
			if (ImGui::BeginMenu("Edit Components"))
			{
				for (const auto& [componentID, componentName, componentCreator] : ECSFactory::GetComponentDatabase())
				{
					bool isAddedAlready = entity->getComponentFromID(componentID) != nullptr;
					if (ImGui::Checkbox(componentName.c_str(), &isAddedAlready))
					{
						if (isAddedAlready)
						{
							if (ECSFactory::AddComponent(entity, ECSFactory::CreateDefaultComponent(componentName)))
							{
								PRINT("Added " + componentName + " to " + entity->getFullName());
							}
						}
						else
						{
							if (entity->removeComponent(componentID))
							{
								PRINT("Removed " + componentName + " from " + entity->getFullName());
							}
						}
					}
				}

				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Reset"))
			{
				PANIC(entity->onAllComponentsAdded() == false, "Could not setup entity: " + entity->getFullName());
			}
			if (ImGui::MenuItem("Remove Entity"))
			{
				m_ActionScene->setEntity(Ptr<Entity>());
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

		if (ImGui::Selectable("Copy Scene"))
		{
			Scene* actionScenePtr = m_ActionScene;
			EventManager::GetSingleton()->defer([=]() {
				if (Ptr<Scene>& copiedScene = Scene::Create(actionScenePtr->getJSON()))
				{
					actionScenePtr->getParent()->addChild(copiedScene);
				}
				else
				{
					WARN("Could not copy from selected scene");
				}
			});
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

				if (ImGui::Button("Add Child Scene From File"))
				{
					if (Optional<String> result = OS::SelectFile("Scene(*.scene.json)\0*.scene.json\0", "game/assets/scenes/"))
					{
						m_OpenedScene->addChild(Scene::CreateFromFile(*result));
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Save Scene to File"))
				{
					if (Optional<String> result = OS::SaveSelectFile("Scene(*.scene.json)\0*.scene.json\0", "game/assets/scenes/"))
					{
						if (!SceneLoader::GetSingleton()->saveSceneAtFile(m_OpenedScene, *result))
						{
							WARN("Could not save selected scene to file: " + *result);
						}
					}
				}

				ImGui::Separator();

				if (Entity* entity = m_OpenedScene->getEntity())
				{
					entity->draw();

					EditorSystem::GetSingleton()->pushBoldFont();
					if (ImGui::BeginTabBar("Components", ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyMask_))
					{
						ImGui::PushStyleColor(ImGuiCol_Text, (const ImVec4&)EditorSystem::GetSingleton()->getNormalColor());
						for (auto& component : entity->getAllComponents())
						{
							if (ImGui::BeginTabItem(component.second->getName(), nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
							{
								EditorSystem::GetSingleton()->pushRegularFont();
								component.second->draw();
								EditorSystem::GetSingleton()->popFont();
								ImGui::EndTabItem();
							}
						}
						ImGui::PopStyleColor();
						ImGui::EndTabBar();
					}
					EditorSystem::GetSingleton()->popFont();
				}
			}
		}
		ImGui::End();
	}
}
