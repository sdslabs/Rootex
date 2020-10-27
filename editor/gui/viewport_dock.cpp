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
	m_ViewportDockSettings.m_ImageTint = EditorSystem::GetSingleton()->getColors().white;
	m_ViewportDockSettings.m_ImageBorderColor = EditorSystem::GetSingleton()->getColors().accent;
	
	m_EditorCamera = Scene::CreateEmptyWithEntity();
	m_EditorCamera->setName("EditorCamera");
	ECSFactory::AddComponent(m_EditorCamera->getEntity(), ECSFactory::CreateDefaultComponent("TransformComponent"));
	ECSFactory::AddComponent(m_EditorCamera->getEntity(), ECSFactory::CreateDefaultComponent("CameraComponent"));
	RenderSystem::GetSingleton()->setCamera(m_EditorCamera->getEntity()->getComponent<CameraComponent>().get());

	m_EditorGrid = Scene::CreateEmptyWithEntity();
	m_EditorGrid->setName("EditorGrid");
	ECSFactory::AddComponent(m_EditorGrid->getEntity(), ECSFactory::CreateDefaultComponent("TransformComponent"));
	ECSFactory::AddComponent(m_EditorGrid->getEntity(), ECSFactory::CreateDefaultComponent("GridModelComponent"));
	
	SceneLoader::GetSingleton()->getRootScene()->addChild(m_EditorCamera);
	SceneLoader::GetSingleton()->getRootScene()->addChild(m_EditorGrid);
}

void FindSelectedEntity(Entity* result, Scene* scene, const Ray& ray, float minimumDistance = D3D11_FLOAT32_MAX)
{
	if (Entity* entity = scene->getEntity())
	{
		if (Ref<TransformComponent> transform = entity->getComponent<TransformComponent>())
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
			    RenderingDevice::GetSingleton()->getOffScreenRTSRVResolved().Get(),
			    m_ViewportDockSettings.m_ImageSize,
			    { 0, 0 },
			    { 1, 1 },
			    m_ViewportDockSettings.m_ImageTint,
			    m_ViewportDockSettings.m_ImageBorderColor);

			ImVec2 imageSize = ImGui::GetItemRectSize();
			ImVec2 imagePos = ImGui::GetItemRectMin();

			InputInterface::s_ScaleX = Application::GetSingleton()->getWindow()->getWidth() / imageSize.x;
			InputInterface::s_ScaleY = Application::GetSingleton()->getWindow()->getHeight() / imageSize.y;
			
			InputInterface::s_Left = imagePos.x;
			InputInterface::s_Right = InputInterface::s_Left + imageSize.x;
			InputInterface::s_Top = imagePos.y;
			InputInterface::s_Bottom = InputInterface::s_Top + imageSize.y;

			static const ImVec2 viewportEnd = ImGui::GetCursorPos();

			static ImGuizmo::OPERATION gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
			static float axisSnap[3] = { 0.1f, 0.1f, 0.1f };
			static float angleSnap = { 45.0f };
			static float scaleSnap = { 0.1f };
			static float* currentSnap = nullptr;
			static ImGuizmo::MODE gizmoMode = ImGuizmo::MODE::LOCAL;
			ImGui::Begin("Viewport Tools");
			{
				ImGui::BeginGroup();
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
				ImGui::EndGroup();

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

				if (ImGui::RadioButton("Local", gizmoMode == ImGuizmo::MODE::LOCAL))
				{
					gizmoMode = ImGuizmo::MODE::LOCAL;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("World", gizmoMode == ImGuizmo::MODE::WORLD))
				{
					gizmoMode = ImGuizmo::MODE::WORLD;
				}

				ImGui::DragFloat("Camera Sensitivity", &m_EditorCameraSensitivity);
				ImGui::DragFloat("Camera Speed", &m_EditorCameraSpeed);
			}
			if (ImGui::TreeNodeEx("RenderSystem"))
			{
				RenderSystem::GetSingleton()->draw();
				ImGui::TreePop();
			}
			ImGui::End();

			Matrix view = RenderSystem::GetSingleton()->getCamera()->getViewMatrix();
			Matrix proj = RenderSystem::GetSingleton()->getCamera()->getProjectionMatrix();

			Scene* openedScene = InspectorDock::GetSingleton()->getOpenedScene();
			if (openedScene && openedScene->getEntity() && openedScene->getEntity()->getComponent<TransformComponent>())
			{
				Entity* openedEntity = openedScene->getEntity();
				TransformComponent* transform = openedEntity->getComponent<TransformComponent>().get();

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

				matrix *= transform->getParentAbsoluteTransform().Invert();

				transform->setTransform(matrix);
			}
			
			if (ImGui::IsWindowHovered())
			{
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
				Entity* selectEntity = nullptr;
				FindSelectedEntity(selectEntity, SceneLoader::GetSingleton()->getRootScene().get(), ray);
				
				if (selectEntity)
				{
					Ref<TransformComponent> transform = selectEntity->getComponent<TransformComponent>();
					transform->highlight();
					ImGui::SetCursorPos({ ImGui::GetMousePos().x - ImGui::GetWindowPos().x + 10.0f, ImGui::GetMousePos().y - ImGui::GetWindowPos().y - 20.0f });
					EditorSystem::GetSingleton()->pushBoldFont();
					ImGui::TextColored(EditorSystem::GetSingleton()->getColors().white, "%s", transform->getOwner()->getFullName().c_str());
					EditorSystem::GetSingleton()->popFont();
				}
				if (selectEntity && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
				{
					EventManager::GetSingleton()->call("MouseSelectEntity", "EditorOpenScene", selectEntity);
					PRINT("Picked entity through selection: " + selectEntity->getFullName());
				}
				if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
				{
					EventManager::GetSingleton()->call("MouseSelectEntity", "EditorCloseScene", 0);
				}
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
