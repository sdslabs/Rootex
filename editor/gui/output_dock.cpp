#include "output_dock.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

Variant OutputDock::catchOutput(const Event* event)
{
	m_CaughtOutputs.push_back(Extract(String, event->getData()));
	return true;
}

OutputDock::OutputDock()
{
	BIND_EVENT_MEMBER_FUNCTION("OSPrint", catchOutput);
}

void OutputDock::draw()
{
	if (m_OutputDockSettings.m_IsActive)
	{
		if (ImGui::Begin("Output"))
		{
			for (auto&& outputString : m_CaughtOutputs)
			{
				ImGui::TextUnformatted(outputString.c_str());
			}
		}
		ImGui::SetScrollHere(1.0f);
		ImGui::End();
	}
}
