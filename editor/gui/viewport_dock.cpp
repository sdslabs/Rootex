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

	m_EditorCamera = EntityFactory::GetSingleton()->createEntity(ResourceLoader::CreateTextResourceFile("editor/entities/camera.entity.json"));
	m_EditorCamera->setEditorOnly(true);
	RenderSystem::GetSingleton()->setCamera(m_EditorCamera->getComponent<CameraComponent>().get());
	m_ApplyCameraMatrix = m_EditorCamera->getComponent<TransformComponent>()->getAbsoluteTransform();
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
			if (ImGui::RadioButton("Scale", gizmoOperation == ImGuizmo::OPERATION::SCALE))
			{
				gizmoOperation = ImGuizmo::OPERATION::SCALE;
			}
			ImGui::EndGroup();

			static float snap[3] = { 0.1f, 0.1f, 0.1f };
			ImGui::SetNextItemWidth(ImGui::GetItemRectSize().x);
			if (gizmoOperation == ImGuizmo::OPERATION::TRANSLATE)
			{
				ImGui::DragFloat3("Axis Snap", snap, 0.1f);
			}
			else if (gizmoOperation == ImGuizmo::OPERATION::ROTATE)
			{
				ImGui::DragFloat("Angle Snap", snap, 0.1f);
			}
			else if (gizmoOperation == ImGuizmo::OPERATION::SCALE)
			{
				ImGui::DragFloat("Scale Snap", snap, 0.1f);
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
			
			ImGui::Unindent(2.0f);
			
			ImGui::SetCursorPos(viewportEnd);
			Ref<Entity> openedEntity = InspectorDock::GetSingleton()->getOpenedEntity();
			if (openedEntity && openedEntity->getComponent<TransformComponent>())
			{
				static ImVec2 rect;
				rect = ImGui::GetWindowSize();
				static ImVec2 pos;
				pos = ImGui::GetWindowPos();
				ImGuizmo::SetRect(pos.x, pos.y, rect.x, rect.y);
				Matrix view = RenderSystem::GetSingleton()->getCamera()->getViewMatrix();
				Matrix proj = RenderSystem::GetSingleton()->getCamera()->getProjectionMatrix();

				Matrix matrix = openedEntity->getComponent<TransformComponent>()->getLocalTransform();
				ImGuizmo::Manipulate(
				    &view.m[0][0],
				    &proj.m[0][0],
				    gizmoOperation,
				    gizmoMode,
				    &matrix.m[0][0],
				    0,
				    snap);
				openedEntity->getComponent<TransformComponent>()->setTransform(matrix);
			}

			if (InputManager::GetSingleton()->isPressed("InputMouseRight"))
			{
				static POINT pos;
				if (!m_IsCameraMoving)
				{
					EditorApplication::GetSingleton()->getWindow()->showCursor(false);
					GetCursorPos(&pos);
					m_IsCameraMoving = true;
				}

				float deltaUp = -InputManager::GetSingleton()->getFloatDelta("InputCameraTurnUp");
				float deltaRight = -InputManager::GetSingleton()->getFloatDelta("InputCameraTurnRight");
				PRINT(std::to_string(deltaRight))
				
				m_ApplyCameraMatrix = Matrix::CreateFromAxisAngle(m_ApplyCameraMatrix.Up(), deltaRight) * m_ApplyCameraMatrix;
				m_ApplyCameraMatrix = Matrix::CreateFromAxisAngle(m_ApplyCameraMatrix.Right(), deltaUp) * m_ApplyCameraMatrix;

				if (InputManager::GetSingleton()->isPressed("InputCameraForward"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(m_ApplyCameraMatrix.Forward() * m_EditorCameraSpeed) * m_ApplyCameraMatrix;
				}
				if (InputManager::GetSingleton()->isPressed("InputCameraBackward"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(m_ApplyCameraMatrix.Backward() * m_EditorCameraSpeed) * m_ApplyCameraMatrix;
				}
				if (InputManager::GetSingleton()->isPressed("InputCameraLeft"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(m_ApplyCameraMatrix.Left() * m_EditorCameraSpeed) * m_ApplyCameraMatrix;
				}
				if (InputManager::GetSingleton()->isPressed("InputCameraRight"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(m_ApplyCameraMatrix.Right() * m_EditorCameraSpeed) * m_ApplyCameraMatrix;
				}
				if (InputManager::GetSingleton()->isPressed("InputCameraUp"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(m_ApplyCameraMatrix.Up() * m_EditorCameraSpeed) * m_ApplyCameraMatrix;
				}
				if (InputManager::GetSingleton()->isPressed("InputCameraDown"))
				{
					m_ApplyCameraMatrix = Matrix::CreateTranslation(m_ApplyCameraMatrix.Down() * m_EditorCameraSpeed) * m_ApplyCameraMatrix;
				}

				//SetCursorPos(pos.x, pos.y);
			}
			else
			{
				if (m_IsCameraMoving)
				{
					EditorApplication::GetSingleton()->getWindow()->showCursor(true);
					m_IsCameraMoving = false;
				}
			}
			m_EditorCamera->getComponent<TransformComponent>()->setTransform(m_ApplyCameraMatrix);
		}
		ImGui::End();
	}
}
