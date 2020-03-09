#include "output_dock.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

void OutputDock::draw()
{
	if (m_OutputDockSettings.m_IsActive)
	{
		if (ImGui::Begin("Output"))
		{
			for (auto&& log : m_Logs)
			{
				ImGui::Text(log.c_str());
			}
		}
		ImGui::End();
	}
}

void OutputDock::log(const String& log)
{
	m_Logs.push_back(log);
}
