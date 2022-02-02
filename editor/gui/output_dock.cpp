#include "output_dock.h"

#include "editor/editor_system.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

#include "imgui_stdlib.h"

Variant OutputDock::catchOutput(const Event* event)
{
	VariantVector data = Extract<VariantVector>(event->getData());
	String msg = Extract<String>(data.front());
	String type = Extract<String>(data.back());
	m_CaughtOutputs.push_back({ type, msg });
	m_IsOutputJustCaught = true;
	return true;
}

OutputDock::OutputDock()
{
	m_Binder.bind(RootexEvents::OSPrint, this, &OutputDock::catchOutput);
}

void OutputDock::draw(float deltaMilliseconds)
{
	ZoneScoped;
	if (m_OutputDockSettings.m_IsActive)
	{
		static unsigned int filter = (int)OutputState::All;

		static bool outputActive;

		if (ImGui::Begin("Output", &outputActive, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::CheckboxFlags("General", &filter, (int)OutputState::General);
				ImGui::CheckboxFlags("Warning", &filter, (int)OutputState::Warning);
				ImGui::CheckboxFlags("Error", &filter, (int)OutputState::Error);
				ImGui::CheckboxFlags("All", &filter, (int)OutputState::All);

				if (ImGui::SmallButton("Clear"))
				{
					m_CaughtOutputs.clear();
				}

				ImGui::EndMenuBar();
			}

			for (auto&& outputString : m_CaughtOutputs)
			{
				if (outputString.first == "Error")
				{
					ImGui::PushStyleColor(ImGuiCol_Text, (const ImVec4&)EditorSystem::GetSingleton()->getFatalColor());
				}
				else if (outputString.first == "Warning")
				{
					ImGui::PushStyleColor(ImGuiCol_Text, (const ImVec4&)EditorSystem::GetSingleton()->getWarningColor());
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Text, (const ImVec4&)EditorSystem::GetSingleton()->getNormalColor());
				}

				if (((filter & (int)OutputState::General) && !(outputString.first == "Warning" || outputString.first == "Error")) || ((filter & (int)OutputState::Warning) && (outputString.first == "Warning")) || ((filter & (int)OutputState::Error) && (outputString.first == "Error")))
				{
					ImGui::TextWrapped("%s", outputString.second.c_str());
				}

				ImGui::PopStyleColor(1);
			}
		}

		static String command;
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
		if (ImGui::InputTextWithHint("##Enter Command", "Command", &command, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			try
			{
				LuaInterpreter::GetSingleton()->getLuaState().safe_script(command);
				command = "";
				ImGui::SetScrollHere(1.0f);
			}
			catch (std::exception e)
			{
				WARN("Script error. Check console for details.");
			}
		}

		if (m_IsOutputJustCaught)
		{
			ImGui::SetScrollHere(1.0f);
			m_IsOutputJustCaught = false;
		}

		ImGui::End();
	}
}
