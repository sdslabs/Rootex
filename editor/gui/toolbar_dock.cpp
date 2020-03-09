#include "toolbar_dock.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

void ToolbarDock::draw()
{
	if (m_ToolbarDockSettings.m_IsActive)
	{
		if (ImGui::Begin("Toolbar"))
		{
		}
		ImGui::End();
	}
}
