#include "scene_dock.h"

#include "editor/editor_system.h"

#include "framework/scene_loader.h"
#include "framework/systems/render_system.h"
#include "framework/components/transform_component.h"
#include "utility/imgui_helpers.h"

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

			ImGui::PushStyleColor(ImGuiCol_Text, (const ImVec4&)EditorSystem::GetSingleton()->getNormalColor());
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
					if (rearrangeScene->findScene(scene->getID()) == nullptr)
					{
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
						Scene* rearrangeScenePtr = rearrangeScene.get();
						Scene* scenePtr = scene.get();
						EventManager::GetSingleton()->defer([=]() {
							scenePtr->snatchChild(rearrangeScenePtr);
						});
						openScene(scenePtr);
					}
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
			if (SceneLoader::GetSingleton()->getCurrentScene() && ImGui::Button("Instantiate Scene"))
			{
				if (Optional<String> result = OS::SelectFile("Scene(*.scene.json)\0*.scene.json\0", "game/assets/scenes/"))
				{
					if (Ptr<Scene>& newScene = Scene::CreateFromFile(*result))
					{
						if (Entity* entity = newScene->getEntity())
						{
							if (TransformComponent* tc = entity->getComponent<TransformComponent>())
							{
								TransformComponent* cameraTransform = RenderSystem::GetSingleton()->getCamera()->getOwner()->getComponent<TransformComponent>();
								tc->setRotationPosition(cameraTransform->getRotationPosition());
							}
						}
						SceneLoader::GetSingleton()->getCurrentScene()->addChild(newScene);
					}
				}
			}

			showSceneTree(SceneLoader::GetSingleton()->getRootSceneEx());
		}
		ImGui::End();
	}
}
