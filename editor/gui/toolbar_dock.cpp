#include "toolbar_dock.h"

#include "editor/editor_application.h"

#include "core/resource_loader.h"
#include "core/renderer/texture.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

void ToolbarDock::draw()
{
	if (m_ToolbarDockSettings.m_IsActive)
	{
		if (ImGui::Begin("Toolbar"))
		{
			ImGui::Columns(2);
			
			ImGui::Text("Play Level");
			ImGui::SameLine();
			if (ImGui::ArrowButton("Play Level", ImGuiDir_Right))
			{
			}

			ImGui::NextColumn();
		
			ImGui::Text("Play Game");
			ImGui::SameLine();
			if (ImGui::ArrowButton("Play Game", ImGuiDir_Right))
			{
				EventManager::GetSingleton()->call("PreGameStartupSaveEvent", "EditorSaveAll", 0);
				PRINT("Launched Game process");
				OS::Execute("\"" + OS::GetGameExecutablePath() + "\"");
				PRINT("Game process ended");
			}

			ImGui::Columns(1);

			if (ImGui::TreeNodeEx("Editor", ImGuiTreeNodeFlags_CollapsingHeader))
			{
				float currentTime = EditorApplication::GetSingleton()->getAppFrameTimer().getTimeMs();
				ImGui::LabelText(std::to_string(currentTime - m_LastUpdateTime).c_str(), "Frame Time");
				ImGui::LabelText(std::to_string((unsigned int)(1.0f / ((currentTime - m_LastUpdateTime) * MS_TO_S))).c_str(), "Frames Per Second");
				m_LastUpdateTime = currentTime;
			}
		}
		ImGui::End();
	}
}
