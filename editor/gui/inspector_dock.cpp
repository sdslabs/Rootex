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
{
	m_Binder.bind(EditorEvents::EditorOpenScene, this, &InspectorDock::openScene);
	m_Binder.bind(EditorEvents::EditorSceneIsClosing, this, &InspectorDock::closeScene);
	m_Binder.bind(EditorEvents::EditorReset, this, &InspectorDock::closeScene);

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
		EventManager::GetSingleton()->call(EditorEvents::EditorOpenScene, m_ActionScene);
	}
	if (m_ActionScene != SceneLoader::GetSingleton()->getRootScene())
	{
		if (ImGui::MenuItem("Reimport"))
		{
			m_ActionScene->reimport();
		}

		Entity& entity = m_ActionScene->getEntity();

		if (ImGui::BeginMenu("Edit Components"))
		{
			static String searchString;
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
			ImGui::SetItemDefaultFocus();
			ImGui::InputText("##SearchComponent", &searchString);

			Vector<String> components;
			for (const auto& [componentName, componentData] : ECSFactory::s_ComponentSets)
			{
				components.push_back(componentName);
			}
			std::sort(components.begin(), components.end());
			for (const auto& componentName : components)
			{
				auto& componentData = ECSFactory::s_ComponentSets[componentName];
				bool shouldMatch = false;
				for (int i = 0; i < componentName.size(); i++)
				{
					shouldMatch |= componentName.compare(i, searchString.size(), searchString) == 0;
				}

				if (shouldMatch)
				{
					bool isAddedAlready = entity.getComponentFromID(componentData->getID()) != nullptr;
					if (ImGui::Checkbox(componentData->getName().c_str(), &isAddedAlready))
					{
						if (isAddedAlready)
						{
							if (ECSFactory::AddDefaultComponent(entity, componentData->getID()))
							{
								PRINT("Added " + componentData->getName() + " to " + entity.getFullName());
							}
						}
						else
						{
							if (entity.removeComponent(componentData->getID()))
							{
								PRINT("Removed " + componentData->getName() + " from " + entity.getFullName());
							}
						}
					}
					ImGui::SameLine();
					ImGui::TextColored(EditorSystem::GetSingleton()->getSuccessColor(), "%s", componentData->getCategory().c_str());
				}
			}

			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Setup"))
		{
			PANIC(entity.onAllComponentsAdded() == false, "Could not setup entity: " + entity.getFullName());
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Add Empty Scene"))
		{
			Ptr<Scene> childScene = Scene::CreateEmpty();
			Scene* childScenePtr = childScene.get();
			m_ActionScene->addChild(childScene);
			EventManager::GetSingleton()->call(EditorEvents::EditorOpenScene, childScenePtr);
		}

		if (ImGui::Selectable("Copy Scene"))
		{
			Scene* actionScenePtr = m_ActionScene;
			EventManager::GetSingleton()->defer([=]() {
				if (Ptr<Scene> copiedScene = Scene::Create(actionScenePtr->getJSON(), true))
				{
					Scene* copiedScenePtr = copiedScene.get();
					actionScenePtr->getParent()->addChild(copiedScene);
					EventManager::GetSingleton()->call(EditorEvents::EditorOpenScene, copiedScenePtr);
				}
				else
				{
					WARN("Could not copy from selected scene");
				}
			});
		}
		if (ImGui::Selectable("Delete Scene"))
		{
			EventManager::GetSingleton()->deferredCall(RootexEvents::DeleteScene, m_ActionScene);
			closeScene(nullptr);
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
				String addition;
				if (m_OpenedScene->getImportStyle() == Scene::ImportStyle::External)
				{
					addition = ICON_ROOTEX_EXTERNAL_LINK;
					ImGui::PushStyleColor(ImGuiCol_Header, (ImVec4)EditorSystem::GetSingleton()->getLinkColor());
				}

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
					ImGui::TreeNodeEx((m_OpenedScene->getFullName() + " " + addition).c_str(), ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Selected);
					if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
					{
						m_IsNameBeingEdited = true;
					}
					EditorSystem::GetSingleton()->popFont();

					if (m_OpenedScene->getImportStyle() == Scene::ImportStyle::External)
					{
						ImGui::TextColored(EditorSystem::GetSingleton()->getLinkColor(), "%s", ("Imported from " + m_OpenedScene->getScenePath()).c_str());
					}
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

				Entity& entity = m_OpenedScene->getEntity();

				entity.draw();

				EditorSystem::GetSingleton()->pushBoldFont();
				if (ImGui::BeginTabBar("Components", ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyMask_))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, (const ImVec4&)EditorSystem::GetSingleton()->getNormalColor());
					for (auto& [componentID, component] : entity.getAllComponents())
					{
						if (ImGui::BeginTabItem(component->getName(), nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
						{
							EditorSystem::GetSingleton()->pushRegularFont();
							component->draw();
							EditorSystem::GetSingleton()->popFont();
							ImGui::EndTabItem();
						}
					}
					ImGui::PopStyleColor();
					ImGui::EndTabBar();
				}
				EditorSystem::GetSingleton()->popFont();

				if (m_OpenedScene->getImportStyle() == Scene::ImportStyle::External)
				{
					ImGui::PopStyleColor();
				}
			}
		}
		ImGui::End();
	}
}
