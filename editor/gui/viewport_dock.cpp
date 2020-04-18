#include "viewport_dock.h"

#include "renderer/rendering_device.h"
#include "framework/systems/render_system.h"

#include "editor/editor.h"
#include "editor/gui/inspector_dock.h"

#include "ImGuizmo.h"

ViewportDock::ViewportDock(const JSON::json& viewportJSON)
{
	m_ViewportDockSettings.m_AspectRatio = (float)viewportJSON["aspectRatio"]["x"] / (float)viewportJSON["aspectRatio"]["y"];
	m_ViewportDockSettings.m_ImageTint = Editor::GetSingleton()->getColors().m_White;
	m_ViewportDockSettings.m_ImageBorderColor = Editor::GetSingleton()->getColors().m_Accent;
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

			static ImVec2 rect;
			rect = ImGui::GetWindowSize();
			static ImVec2 pos;
			pos = ImGui::GetWindowPos();
			ImGuizmo::SetRect(pos.x, pos.y, rect.x, rect.y);
			Matrix view = RenderSystem::GetSingleton()->getCamera()->getViewMatrix();
			Matrix proj = RenderSystem::GetSingleton()->getCamera()->getProjectionMatrix();
			static Matrix gridLocation = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);

			ImGuizmo::DrawGrid(
				&view.m[0][0],
				&proj.m[0][0],
				&gridLocation.m[0][0],
				2.0f);

			ImGui::Image(
			    RenderingDevice::GetSingleton()->getRenderTextureShaderResourceView().Get(),
			    m_ViewportDockSettings.m_ImageSize,
			    { 0, 0 },
			    { 1, 1 },
			    m_ViewportDockSettings.m_ImageTint,
			    m_ViewportDockSettings.m_ImageBorderColor);

	
			Ref<Entity> openedEntity = InspectorDock::GetSingleton()->getOpenedEntity();
			if (openedEntity && openedEntity->getComponent<TransformComponent>())
			{
				Matrix matrix = openedEntity->getComponent<TransformComponent>()->getAbsoluteTransform();
				float snap[3] = { 1.0f, 1.0f, 1.0f };
				ImGuizmo::Manipulate(
				    &view.m[0][0],
				    &proj.m[0][0],
				    ImGuizmo::OPERATION::TRANSLATE,
				    ImGuizmo::MODE::LOCAL,
				    &matrix.m[0][0],
				    0,
				    snap);
				openedEntity->getComponent<TransformComponent>()->setTransform(matrix);
			}
		}
		ImGui::End();
	}
}
