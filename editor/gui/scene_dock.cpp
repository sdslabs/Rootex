#include "scene_dock.h"

#include "editor/editor_system.h"

#include "framework/scene_loader.h"
#include "framework/components/transform_component.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

void SceneDock::showSceneTree(Ptr<Scene>& scene)
{
	ZoneScoped;
	
	static int uniqueID = 0;
	uniqueID++;
	if (scene)
	{
		ImGui::PushID(uniqueID);
		if (ImGui::TreeNodeEx("", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | (scene->getChildren().size() ? ImGuiTreeNodeFlags_None : ImGuiTreeNodeFlags_Leaf)))
		{
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Text, EditorSystem::GetSingleton()->getColors().text);
			if (ImGui::Selectable(scene->getFullName().c_str(), m_OpenedSceneID == scene->getID()))
			{
				openScene(scene.get());
			}

			if (ImGui::BeginPopupContextItem())
			{
				openScene(scene.get());
				InspectorDock::GetSingleton()->drawSceneActions(scene.get());
				ImGui::EndPopup();
			}

			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("RearrangeScene", &scene, sizeof(Ptr<Scene>));
				ImGui::Text(scene->getFullName().c_str());
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RearrangeScene"))
				{
					Ptr<Scene>& rearrangeScene = *(Ptr<Scene>*)(payload->Data);
					scene->snatchChild(rearrangeScene);
					Entity* victimEntity = rearrangeScene->getEntity();
					Entity* thiefEntity = scene->getEntity();
					if (victimEntity && thiefEntity)
					{
						TransformComponent* victimTransform = victimEntity->getComponent<TransformComponent>();
						TransformComponent* thiefTransform = thiefEntity->getComponent<TransformComponent>();
						if (victimTransform && thiefTransform)
						{
							victimTransform->setTransform(victimTransform->getAbsoluteTransform() * thiefTransform->getAbsoluteTransform().Invert());
						}
					}
					openScene(scene.get());
				}
				ImGui::EndDragDropTarget();
			}
			
			ImGui::PopStyleColor(1);

			for (auto& child : scene->getChildren())
			{
				showSceneTree(child);
			}

			ImGui::TreePop();
		}
		ImGui::PopID();
	}
	uniqueID--;
}

void SceneDock::openScene(Scene* scene)
{
	m_OpenedSceneID = scene->getID();
	EventManager::GetSingleton()->call("OpenEntity", "EditorOpenScene", scene);
}

Variant SceneDock::selectOpenScene(const Event* event)
{
	m_OpenedSceneID = Extract<Scene*>(event->getData())->getID();
	return true;
}

SceneDock::SceneDock()
{
	BIND_EVENT_MEMBER_FUNCTION("EditorOpenScene", SceneDock::selectOpenScene);
}

void SceneDock::draw(float deltaMilliseconds)
{
	ZoneScoped;
	if (m_SceneDockSettings.m_IsActive)
	{
		if (ImGui::Begin("Scene"))
		{
			showSceneTree(SceneLoader::GetSingleton()->getRootSceneEx());
		}
		ImGui::End();
	}
	if (m_SceneDockSettings.m_IsEntitiesDockActive)
	{
		if (ImGui::Begin("Entities"))
		{
			ImGui::Columns(2);
			ImGui::Text("Entity");
			ImGui::NextColumn();
			ImGui::Text("Components");
			ImGui::NextColumn();

			showEntities(SceneLoader::GetSingleton()->getRootScene());

			ImGui::Columns(1);
		}
		ImGui::End();
	}
}

void SceneDock::showEntities(Scene* scene)
{
	if (Entity* entity = scene->getEntity())
	{
		if (ImGui::Selectable(entity->getFullName().c_str(), m_OpenedSceneID == scene->getID()))
		{
			openScene(scene);
		}
		if (ImGui::BeginPopupContextItem())
		{
			InspectorDock::GetSingleton()->drawSceneActions(scene);
			ImGui::EndPopup();
		}
		ImGui::NextColumn();
		const HashMap<ComponentID, Ptr<Component>>& components = entity->getAllComponents();
		HashMap<ComponentID, Ptr<Component>>::const_iterator it = components.begin();
		while (it != components.end())
		{
			bool increment = true;
			Component* component = it->second.get();
			ImGui::Selectable(component->getName());
			if (ImGui::BeginPopupContextItem(("Delete" + entity->getFullName() + component->getName()).c_str()))
			{
				if (ImGui::Button("Delete Component"))
				{
					if (component)
					{
						String componentName = component->getName();
						if (entity->removeComponent(component->getComponentID()))
						{
							PRINT("Deleted " + componentName + " from " + entity->getName());
						}
						increment = false;
					}
					else
					{
						ERR("Component not found: Possible level files corruption");
					}
				}
				ImGui::EndPopup();
			}
			if (increment)
			{
				it++;
			}
		}
		ImGui::NextColumn();
	}

	for (auto& child : scene->getChildren())
	{
		showEntities(child.get());
	}
}
