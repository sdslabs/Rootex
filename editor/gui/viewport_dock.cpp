#include "viewport_dock.h"

#include "renderer/rendering_device.h"
#include "input/input_manager.h"
#include "ui/input_interface.h"
#include "framework/entity.h"
#include "framework/ecs_factory.h"
#include "framework/scene_loader.h"
#include "framework/systems/render_system.h"

#include "editor/editor_system.h"
#include "editor/editor_application.h"
#include "editor/gui/inspector_dock.h"

#include "ImGuizmo.h"

ViewportDock::ViewportDock(const JSON::json& viewportJSON)
    : m_IsCameraMoving(false)
{
	m_ViewportDockSettings.m_AspectRatio = (float)viewportJSON["aspectRatio"]["x"] / (float)viewportJSON["aspectRatio"]["y"];
	
	Ptr<Scene>& editorCamera = Scene::CreateEmptyWithEntity();
	editorCamera->setName("EditorCamera");
	m_EditorCamera = editorCamera.get();
	ECSFactory::AddComponent(m_EditorCamera->getEntity(), ECSFactory::CreateDefaultComponent("TransformComponent"));
	ECSFactory::AddComponent(m_EditorCamera->getEntity(), ECSFactory::CreateDefaultComponent("CameraComponent"));
	RenderSystem::GetSingleton()->setCamera(m_EditorCamera->getEntity()->getComponent<CameraComponent>());
	SceneLoader::GetSingleton()->getRootScene()->addChild(editorCamera);

	Ptr<Scene>& editorGrid = Scene::CreateEmptyWithEntity();
	editorGrid->setName("EditorGrid");
	m_EditorGrid = editorGrid.get();
	ECSFactory::AddComponent(m_EditorGrid->getEntity(), ECSFactory::CreateDefaultComponent("TransformComponent"));
	ECSFactory::AddComponent(m_EditorGrid->getEntity(), ECSFactory::CreateDefaultComponent("GridModelComponent"));
	SceneLoader::GetSingleton()->getRootScene()->addChild(editorGrid);
}

void FindSelectedEntity(Entity*& result, Scene* scene, const Ray& ray, float minimumDistance)
{
	if (Entity* entity = scene->getEntity())
	{
		if (TransformComponent* transform = entity->getComponent<TransformComponent>())
		{
			static float distance = 0.0f;
			BoundingBox boundingBox = transform->getWorldSpaceBounds();
			if (ray.Intersects(boundingBox, distance))
			{
				if (0.0f < distance && distance < minimumDistance)
				{
					minimumDistance = distance;
					result = entity;
				}
			}
		}
	}
	for (auto& child : scene->getChildren())
	{
		FindSelectedEntity(result, child.get(), ray, minimumDistance);
	}
}

void ViewportDock::draw(float deltaMilliseconds)
{
	ZoneScoped;
	if (m_ViewportDockSettings.m_IsActive)
	{
		ImGui::SetNextWindowBgAlpha(1.0f);
		if (ImGui::Begin("Viewport"))
		{
			const ImVec2 windowStart = ImGui::GetCursorPos();

			ImVec2 region = ImGui::GetContentRegionAvail();
			if (region.x / region.y < m_ViewportDockSettings.m_AspectRatio)
			{
				region.y = region.x / m_ViewportDockSettings.m_AspectRatio;
			}
			else
			{
				region.x = region.y * m_ViewportDockSettings.m_AspectRatio;
			}

			m_ViewportDockSettings.m_ImageSize = region;

			static const ImVec2 viewportStart = ImGui::GetCursorPos();
			ImGui::Image(
			    RenderingDevice::GetSingleton()->getOffScreenSRV().Get(),
			    m_ViewportDockSettings.m_ImageSize,
			    { 0, 0 },
			    { 1, 1 });
			static const ImVec2 viewportEnd = ImGui::GetCursorPos();
			
			ImVec2 imageSize = ImGui::GetItemRectSize();
			ImVec2 imagePos = ImGui::GetItemRectMin();

			InputInterface::s_ScaleX = Application::GetSingleton()->getWindow()->getWidth() / imageSize.x;
			InputInterface::s_ScaleY = Application::GetSingleton()->getWindow()->getHeight() / imageSize.y;
			
			InputInterface::s_Left = imagePos.x;
			InputInterface::s_Right = InputInterface::s_Left + imageSize.x;
			InputInterface::s_Top = imagePos.y;
			InputInterface::s_Bottom = InputInterface::s_Top + imageSize.y;

			ImGui::SetCursorPos({ windowStart.x + 3, windowStart.y + 3 });
			static ImGuizmo::OPERATION gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
			static float axisSnap[3] = { 0.1f, 0.1f, 0.1f };
			static float angleSnap = { 45.0f };
			static float scaleSnap = { 0.1f };
			static float* currentSnap = nullptr;
			static ImGuizmo::MODE gizmoMode = ImGuizmo::MODE::LOCAL;

			if (ImGui::Button(ICON_ROOTEX_PICTURE_O "##Viewport Tools Button"))
			{
				ImGui::OpenPopup("Viewport Tools");
			}
			if (ImGui::BeginPopup("Viewport Tools"))
			{
				if (ImGui::RadioButton("Translate", gizmoOperation == ImGuizmo::OPERATION::TRANSLATE))
				{
					gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Rotate", gizmoOperation == ImGuizmo::OPERATION::ROTATE))
				{
					gizmoOperation = ImGuizmo::OPERATION::ROTATE;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Scale", gizmoOperation == ImGuizmo::OPERATION::SCALE))
				{
					gizmoOperation = ImGuizmo::OPERATION::SCALE;
				}

				if (ImGui::RadioButton("Local", gizmoMode == ImGuizmo::MODE::LOCAL))
				{
					gizmoMode = ImGuizmo::MODE::LOCAL;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("World", gizmoMode == ImGuizmo::MODE::WORLD))
				{
					gizmoMode = ImGuizmo::MODE::WORLD;
				}

				if (gizmoOperation == ImGuizmo::OPERATION::TRANSLATE)
				{
					ImGui::DragFloat3("Axis Snap", axisSnap, 0.1f);
					currentSnap = axisSnap;
				}
				else if (gizmoOperation == ImGuizmo::OPERATION::ROTATE)
				{
					ImGui::DragFloat("Angle Snap", &angleSnap, 0.1f);
					currentSnap = &angleSnap;
				}
				else if (gizmoOperation == ImGuizmo::OPERATION::SCALE)
				{
					ImGui::DragFloat("Scale Snap", &scaleSnap, 0.1f);
					currentSnap = &scaleSnap;
				}

				if (ImGui::BeginCombo("Camera", RenderSystem::GetSingleton()->getCamera()->getOwner()->getFullName().c_str()))
				{
					for (auto& c : ECSFactory::GetComponents<CameraComponent>())
					{
						CameraComponent* camera = (CameraComponent*)c;

						if (ImGui::MenuItem(camera->getOwner()->getFullName().c_str()))
						{
							RenderSystem::GetSingleton()->setCamera(camera);
						}
					}
					ImGui::EndCombo();
				}

				ImGui::EndPopup();
			}
			ImGui::SetCursorPos(viewportStart);
		
			Matrix view = RenderSystem::GetSingleton()->getCamera()->getViewMatrix();
			Matrix proj = RenderSystem::GetSingleton()->getCamera()->getProjectionMatrix();

			Scene* openedScene = InspectorDock::GetSingleton()->getOpenedScene();
			if (openedScene && openedScene->getEntity() && openedScene->getEntity()->getComponent<TransformComponent>())
			{
				Entity* openedEntity = openedScene->getEntity();
				TransformComponent* transform = openedEntity->getComponent<TransformComponent>();

				ImGuizmo::SetRect(imagePos.x, imagePos.y, m_ViewportDockSettings.m_ImageSize.x, m_ViewportDockSettings.m_ImageSize.y);

				Matrix matrix = transform->getAbsoluteTransform();
				Matrix deltaMatrix = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);

				ImGuizmo::Manipulate(
					&view.m[0][0],
					&proj.m[0][0],
					gizmoOperation,
					gizmoMode,
					&matrix.m[0][0],
					&deltaMatrix.m[0][0],
					currentSnap);

				if (ImGuizmo::IsUsing())
				{
					matrix *= transform->getParentAbsoluteTransform().Invert();
					transform->setTransform(matrix);
				}
			}
			
			static Entity* selectEntity = nullptr;
			if (ImGui::IsWindowHovered())
			{
				if (!ImGui::IsAnyMouseDown())
				{
					if (InputManager::GetSingleton()->getKeyboard()->GetBoolPrevious(KeyboardButton::KeyQ))
					{
						gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
					}
					if (InputManager::GetSingleton()->getKeyboard()->GetBoolPrevious(KeyboardButton::KeyW))
					{
						gizmoOperation = ImGuizmo::OPERATION::ROTATE;
					}
					if (InputManager::GetSingleton()->getKeyboard()->GetBoolPrevious(KeyboardButton::KeyE))
					{
						gizmoOperation = ImGuizmo::OPERATION::SCALE;
					}
				}

				Vector3 mouseFromWindow;
				{
					ImVec2 mouseFromWindowImGui;
					mouseFromWindowImGui.x = ImGui::GetMousePos().x - imagePos.x;
					mouseFromWindowImGui.y = ImGui::GetMousePos().y - imagePos.y;
					mouseFromWindow = { 2.0f * mouseFromWindowImGui.x / imageSize.x - 1.0f, (2.0f * mouseFromWindowImGui.y / imageSize.y - 1.0f) * -1.0f, 0.0f };
				}

				Vector3 origin = DirectX::XMVector3Unproject(
				    { ImGui::GetMousePos().x - imagePos.x, ImGui::GetMousePos().y - imagePos.y, 0.0f },
				    0,
				    0,
				    imageSize.x,
				    imageSize.y,
				    0,
				    1,
				    proj,
				    view,
				    Matrix::Identity);

				Vector3 dest = DirectX::XMVector3Unproject(
				    { ImGui::GetMousePos().x - imagePos.x, ImGui::GetMousePos().y - imagePos.y, 1.0f },
				    0,
				    0,
				    imageSize.x,
				    imageSize.y,
				    0,
				    1,
				    proj,
				    view,
				    Matrix::Identity);

				Vector3 direction = dest - origin;
				direction.Normalize();

				Ray ray(origin, direction);
				if (Scene* currentScene = SceneLoader::GetSingleton()->getCurrentScene())
				{
					selectEntity = nullptr;
					FindSelectedEntity(selectEntity, currentScene, ray, D3D11_FLOAT32_MAX);
				}
				if (selectEntity && !ImGuizmo::IsUsing())
				{
					TransformComponent* transform = selectEntity->getComponent<TransformComponent>();
					transform->highlight();
					
					ImGui::SetCursorPos({ ImGui::GetMousePos().x - ImGui::GetWindowPos().x + 10.0f, ImGui::GetMousePos().y - ImGui::GetWindowPos().y - 20.0f });
					EditorSystem::GetSingleton()->pushBoldFont();
					ImGui::TextColored(EditorSystem::GetSingleton()->getNormalColor(), "%s", transform->getOwner()->getFullName().c_str());
					EditorSystem::GetSingleton()->popFont();
					
					if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						EventManager::GetSingleton()->call("MouseSelectEntity", "EditorOpenScene", selectEntity->getScene());
						PRINT("Picked entity through selection: " + selectEntity->getFullName());
					}
				}

				if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
				{
					EventManager::GetSingleton()->call("MouseSelectEntity", "EditorCloseScene", 0);
				}
			}

			if (selectEntity && ImGui::BeginPopupContextWindow("Entity Shortcuts"))
			{
				EventManager::GetSingleton()->call("MouseSelectEntity", "EditorOpenScene", selectEntity->getScene());
				InspectorDock::GetSingleton()->drawSceneActions(selectEntity->getScene());
				ImGui::EndPopup();
			}

			if (ImGui::IsWindowHovered() && InputManager::GetSingleton()->isPressed("InputCameraActivate"))
			{
				static POINT cursorWhenActivated;
				if (!m_IsCameraMoving)
				{
					EditorApplication::GetSingleton()->getWindow()->showCursor(false);

					static RECT clip;
					clip.left = ImGui::GetWindowPos().x;
					clip.top = ImGui::GetWindowPos().y;
					clip.right = clip.left + ImGui::GetWindowSize().x;
					clip.bottom = clip.top + ImGui::GetWindowSize().y;

					EditorApplication::GetSingleton()->getWindow()->clipCursor(clip);

					GetCursorPos(&cursorWhenActivated);
					m_IsCameraMoving = true;
				}

				static POINT currentCursor;
				GetCursorPos(&currentCursor);

				float deltaUp = cursorWhenActivated.y - currentCursor.y;
				float deltaRight = cursorWhenActivated.x - currentCursor.x;

				m_EditorCameraPitch += deltaUp * deltaMilliseconds * 1e-3;
				m_EditorCameraYaw += deltaRight * deltaMilliseconds * 1e-3;

				SetCursorPos(cursorWhenActivated.x, cursorWhenActivated.y);

				m_EditorCamera->getEntity()->getComponent<TransformComponent>()->setRotation(
				    m_EditorCameraYaw * m_EditorCameraSensitivity / m_EditorCameraRotationNormalizer,
				    m_EditorCameraPitch * m_EditorCameraSensitivity / m_EditorCameraRotationNormalizer,
				    0.0f);

				m_ApplyCameraMatrix = m_EditorCamera->getEntity()->getComponent<TransformComponent>()->getLocalTransform();

				static const Vector3& forward = { 0.0f, 0.0f, -1.0f };
				static const Vector3& right = { 1.0f, 0.0f, 0.0f };
				if (InputManager::GetSingleton()->isPressed("InputCameraForward"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(forward * m_EditorCameraSpeed * deltaMilliseconds * 1e-3) * m_ApplyCameraMatrix;
				}
				if (InputManager::GetSingleton()->isPressed("InputCameraBackward"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(-forward * m_EditorCameraSpeed * deltaMilliseconds * 1e-3) * m_ApplyCameraMatrix;
				}
				if (InputManager::GetSingleton()->isPressed("InputCameraLeft"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(-right * m_EditorCameraSpeed * deltaMilliseconds * 1e-3) * m_ApplyCameraMatrix;
				}
				if (InputManager::GetSingleton()->isPressed("InputCameraRight"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(right * m_EditorCameraSpeed * deltaMilliseconds * 1e-3) * m_ApplyCameraMatrix;
				}
				if (InputManager::GetSingleton()->isPressed("InputCameraUp"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(Vector3(0.0f, 1.0f, 0.0f) * m_EditorCameraSpeed * deltaMilliseconds * 1e-3) * m_ApplyCameraMatrix;
				}
				if (InputManager::GetSingleton()->isPressed("InputCameraDown"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(Vector3(0.0f, -1.0f, 0.0f) * m_EditorCameraSpeed * deltaMilliseconds * 1e-3) * m_ApplyCameraMatrix;
				}
			}
			else
			{
				if (m_IsCameraMoving)
				{
					EditorApplication::GetSingleton()->getWindow()->showCursor(true);
					EditorApplication::GetSingleton()->getWindow()->resetClipCursor();
					m_IsCameraMoving = false;
				}
			}
			m_EditorCamera->getEntity()->getComponent<TransformComponent>()->setPosition(m_ApplyCameraMatrix.Translation());
		}
		ImGui::End();
	}
}
