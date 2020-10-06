#include "output_dock.h"

#include "editor/editor_system.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

#include "imgui_stdlib.h"

Variant OutputDock::catchOutput(const Event* event)
{
	m_CaughtOutputs.push_back({ event->getName(), Extract(String, event->getData()) });
	m_IsOutputJustCaught = true;
	return true;
}

OutputDock::OutputDock()
{
	BIND_EVENT_MEMBER_FUNCTION("OSPrint", catchOutput);
}

void OutputDock::draw(float deltaMilliseconds)
{
	ZoneScoped;
	if (m_OutputDockSettings.m_IsActive)
	{
		if (ImGui::Begin("Output"))
		{
			for (auto&& outputString : m_CaughtOutputs)
			{
				if (outputString.first == "Error")
				{
					ImGui::PushStyleColor(ImGuiCol_Text, EditorSystem::GetSingleton()->getColors().failure);
				}
				else if (outputString.first == "Warning")
				{
					ImGui::PushStyleColor(ImGuiCol_Text, EditorSystem::GetSingleton()->getColors().warning);
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Text, EditorSystem::GetSingleton()->getColors().text);
				}

				ImGui::TextUnformatted(outputString.second.c_str());

				ImGui::PopStyleColor(1);
			}
		}

		static String command;
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
		if (ImGui::InputText("##Enter Command", &command, ImGuiInputTextFlags_AlwaysInsertMode | ImGuiInputTextFlags_EnterReturnsTrue))
		{
			Vector<String> tokens;
			tokens.resize(2);
			std::istringstream stream(command);
			String token;
			while (std::getline(stream, token, ' '))
			{
				tokens.push_back(token);
			}
			EventManager::GetSingleton()->call("EditorOutputDock", tokens[0], tokens[1]);

			command = "";
		}

		if (m_IsOutputJustCaught)
		{
			ImGui::SetScrollHere(1.0f);
			m_IsOutputJustCaught = false;
		}
		ImGui::End();
	}
}
