#include "output_dock.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

OutputDock::OutputDock()
    : m_CoutRedirect(m_StdOutBuffer.rdbuf())
{
}

void OutputDock::draw()
{
	if (m_OutputDockSettings.m_IsActive)
	{
		if (ImGui::Begin("Output"))
		{
			ImGui::TextUnformatted(m_StdOutBuffer.str().c_str());
		}
		ImGui::End();
	}
}
