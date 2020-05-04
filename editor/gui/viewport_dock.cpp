#include "viewport_dock.h"

#include "renderer/rendering_device.h"
#include "framework/systems/render_system.h"
#include "input/input_manager.h"

#include "editor/editor.h"
#include "editor/editor_application.h"
#include "editor/gui/inspector_dock.h"

#include "ImGuizmo.h"

ViewportDock::ViewportDock(const JSON::json& viewportJSON)
    : m_IsCameraMoving(false)
{
	m_ViewportDockSettings.m_AspectRatio = (float)viewportJSON["aspectRatio"]["x"] / (float)viewportJSON["aspectRatio"]["y"];
	m_ViewportDockSettings.m_ImageTint = Editor::GetSingleton()->getColors().m_White;
	m_ViewportDockSettings.m_ImageBorderColor = Editor::GetSingleton()->getColors().m_Accent;
	
	m_EditorCamera = EntityFactory::GetSingleton()->createEntity(ResourceLoader::CreateTextResourceFile("editor/entities/camera.entity.json"), true);
	RenderSystem::GetSingleton()->setCamera(m_EditorCamera->getComponent<CameraComponent>().get());

	m_EditorGrid = EntityFactory::GetSingleton()->createEntity(ResourceLoader::CreateTextResourceFile("editor/entities/grid.entity.json"), true);
}

void ViewportDock::draw()
{
	if (m_ViewportDockSettings.m_IsActive)
	{
		ImGui::SetNextWindowBgAlpha(1.0f);
		if (ImGui::Begin("Viewport"))
		{
			ImVec2 region = ImGui::GetContentRegionAvail();
			if (region.x / region.y != m_ViewportDockSettings.m_AspectRatio)
			{
				region.y = region.x / m_ViewportDockSettings.m_AspectRatio;
			}

			m_ViewportDockSettings.m_ImageSize = region;

			static const ImVec2 viewportStart = ImGui::GetCursorPos();

			ImGui::Image(
			    RenderingDevice::GetSingleton()->getRenderTextureShaderResourceView().Get(),
			    m_ViewportDockSettings.m_ImageSize,
			    { 0, 0 },
			    { 1, 1 },
			    m_ViewportDockSettings.m_ImageTint,
			    m_ViewportDockSettings.m_ImageBorderColor);

			ImVec2 imageSize = ImGui::GetItemRectSize();
			ImVec2 imagePos = ImGui::GetItemRectMin();
			
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityClass"))
				{
					const char* newEntityFile = (const char*)payload->Data;
					Ref<Entity> entity = EntityFactory::GetSingleton()->createEntity(ResourceLoader::CreateTextResourceFile(newEntityFile));
					HierarchySystem::GetSingleton()->getRootHierarchyComponent()->addChild(entity);
					if (Ref<TransformComponent> transform = entity->getComponent<TransformComponent>())
					{
						transform->setTransform(RenderSystem::GetSingleton()->getCamera()->getOwner()->getComponent<TransformComponent>()->getAbsoluteTransform());
					}
				}
				ImGui::EndDragDropTarget();
			}

			static const ImVec2 viewportEnd = ImGui::GetCursorPos();

			ImGui::SetCursorPos(viewportStart);
			static ImGuizmo::OPERATION gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
			ImGui::Indent(2.0f);

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
			ImGui::EndGroup();

			static float snap[3] = { 0.1f, 0.1f, 0.1f };
			static ImVec2 shortItemSize = ImGui::GetItemRectSize();
			ImGui::SetNextItemWidth(shortItemSize.x);
			if (gizmoOperation == ImGuizmo::OPERATION::TRANSLATE)
			{
				ImGui::DragFloat3("Axis Snap", snap, 0.1f);
			}
			else if (gizmoOperation == ImGuizmo::OPERATION::ROTATE)
			{
				ImGui::DragFloat("Angle Snap", snap, 0.1f);
			}

			static ImGuizmo::MODE gizmoMode = ImGuizmo::MODE::LOCAL;
			if (ImGui::RadioButton("Local", gizmoMode == ImGuizmo::MODE::LOCAL))
			{
				gizmoMode = ImGuizmo::MODE::LOCAL;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("World", gizmoMode == ImGuizmo::MODE::WORLD))
			{
				gizmoMode = ImGuizmo::MODE::WORLD;
			}
			
			ImGui::SetNextItemWidth(shortItemSize.x);
			ImGui::SliderFloat("Camera Sensitivity", &m_EditorCameraSensitivity, 0.1f, 1.5f);
			ImGui::SetNextItemWidth(shortItemSize.x);
			ImGui::SliderFloat("Camera Speed", &m_EditorCameraSpeed, 0.1f, 50.0f);

			ImGui::Unindent(2.0f);
			
			ImGui::SetCursorPos(viewportEnd);
			Ref<Entity> openedEntity = InspectorDock::GetSingleton()->getOpenedEntity();
			if (openedEntity && openedEntity->getComponent<TransformComponent>())
			{
				// +8 for 8 pixels of gap, and it fixes the unwanted offset on the gizmo
				ImGuizmo::SetRect(imagePos.x, imagePos.y + 8, m_ViewportDockSettings.m_ImageSize.x, m_ViewportDockSettings.m_ImageSize.y);
				Matrix view = RenderSystem::GetSingleton()->getCamera()->getViewMatrix();
				Matrix proj = RenderSystem::GetSingleton()->getCamera()->getProjectionMatrix();

				Matrix matrix = openedEntity->getComponent<TransformComponent>()->getAbsoluteTransform();
				Matrix deltaMatrix = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
				ImGuizmo::Manipulate(
				    &view.m[0][0],
				    &proj.m[0][0],
				    gizmoOperation,
				    gizmoMode,
				    &matrix.m[0][0],
				    &deltaMatrix.m[0][0],
				    snap);
				openedEntity->getComponent<TransformComponent>()->addTransform(deltaMatrix);
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
				
				m_EditorCameraPitch += deltaUp;
				m_EditorCameraYaw += deltaRight;

				SetCursorPos(cursorWhenActivated.x, cursorWhenActivated.y);

				m_EditorCamera->getComponent<TransformComponent>()->setRotation(
					m_EditorCameraYaw * m_EditorCameraSensitivity / m_EditorCameraRotationNormalizer,
					m_EditorCameraPitch * m_EditorCameraSensitivity / m_EditorCameraRotationNormalizer, 
					0.0f);
				
				m_ApplyCameraMatrix = m_EditorCamera->getComponent<TransformComponent>()->getLocalTransform();

				static const Vector3& forward = { 0.0f, 0.0f, -1.0f };
				static const Vector3& right = { 1.0f, 0.0f, 0.0f };

				if (InputManager::GetSingleton()->isPressed("InputCameraForward"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(forward * m_EditorCameraSpeed * EditorApplication::GetSingleton()->getAppFrameTimer().getLastFrameTime() * MS_TO_S) * m_ApplyCameraMatrix;
				}
				if (InputManager::GetSingleton()->isPressed("InputCameraBackward"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(-forward * m_EditorCameraSpeed * EditorApplication::GetSingleton()->getAppFrameTimer().getLastFrameTime() * MS_TO_S) * m_ApplyCameraMatrix;
				}
				if (InputManager::GetSingleton()->isPressed("InputCameraLeft"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(-right * m_EditorCameraSpeed * EditorApplication::GetSingleton()->getAppFrameTimer().getLastFrameTime() * MS_TO_S) * m_ApplyCameraMatrix;
				}
				if (InputManager::GetSingleton()->isPressed("InputCameraRight"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(right * m_EditorCameraSpeed * EditorApplication::GetSingleton()->getAppFrameTimer().getLastFrameTime() * MS_TO_S) * m_ApplyCameraMatrix;
				}
				if (InputManager::GetSingleton()->isPressed("InputCameraUp"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(Vector3(0.0f, 1.0f, 0.0f) * m_EditorCameraSpeed * EditorApplication::GetSingleton()->getAppFrameTimer().getLastFrameTime() * MS_TO_S) * m_ApplyCameraMatrix;
				}
				if (InputManager::GetSingleton()->isPressed("InputCameraDown"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(Vector3(0.0f, -1.0f, 0.0f) * m_EditorCameraSpeed * EditorApplication::GetSingleton()->getAppFrameTimer().getLastFrameTime() * MS_TO_S) * m_ApplyCameraMatrix;
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
			m_EditorCamera->getComponent<TransformComponent>()->setPosition(m_ApplyCameraMatrix.Translation());
		}
		ImGui::End();
	}
}
