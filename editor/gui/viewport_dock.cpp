#include "viewport_dock.h"

#include "renderer/rendering_device.h"

ViewportDock::ViewportDock(LuaVariable viewportSettings)
{
	m_ViewportDockSettings.m_AspectRatio = (float)LuaInterpreter::GetSingleton()->getGlobal("viewport")["aspectRatio"];
	m_ViewportDockSettings.m_ImageTint = {
		(float)viewportSettings["imageTint"]["r"],
		(float)viewportSettings["imageTint"]["g"],
		(float)viewportSettings["imageTint"]["b"],
		(float)viewportSettings["imageTint"]["a"],
	};
	m_ViewportDockSettings.m_ImageBorderColor = {
		(float)viewportSettings["borderColor"]["r"],
		(float)viewportSettings["borderColor"]["g"],
		(float)viewportSettings["borderColor"]["b"],
		(float)viewportSettings["borderColor"]["a"],
	};
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

			ImGui::Image(
			    RenderingDevice::GetSingleton()->getRenderTextureShaderResourceView(),
			    m_ViewportDockSettings.m_ImageSize,
			    { 0, 0 },
			    { 1, 1 },
			    m_ViewportDockSettings.m_ImageTint,
			    m_ViewportDockSettings.m_ImageBorderColor);
		}
		ImGui::End();
	}
}
