#include "toolbar_dock.h"

#include "editor/editor_application.h"

#include "app/level_manager.h"
#include "framework/system.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_stdlib.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

ToolbarDock::ToolbarDock()
{
	m_FPSRecords.resize(m_FPSRecordsPoolSize, 0.0f);
}

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
				EventManager::GetSingleton()->call("PreGameStartupSaveEvent", "EditorSaveAll", 0);
				PRINT("Launched Game process");
				OS::Execute("\"" + OS::GetGameExecutablePath() + "\" " + LevelManager::GetSingleton()->getCurrentLevelName());
				PRINT("Game process ended");
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
				m_FPSRecords.erase(m_FPSRecords.begin());
				m_FPSRecords.push_back(EditorApplication::GetSingleton()->getAppFrameTimer().getLastFPS());
				
				static float averageFPS = 0.0f;
				for (auto& fps : m_FPSRecords)
				{
					averageFPS += fps;
				}
				averageFPS /= m_FPSRecords.size();

				ImGui::PlotLines("FPS", m_FPSRecords.data(), m_FPSRecords.size(), 0, std::to_string(averageFPS).c_str(), 0, 200.0f, ImVec2(ImGui::GetContentRegionAvailWidth(), 100));
			}

			if (ImGui::TreeNodeEx("Events", ImGuiTreeNodeFlags_CollapsingHeader))
			{
				for (auto&& [eventType, eventHandlers] : EventManager::GetSingleton()->getRegisteredEvents())
				{				
					ImGui::Text((eventType + " (" + std::to_string(eventHandlers.size()) + ")").c_str());
				}
			}

			for (auto& [order, systems] : System::GetSystems()) 
			{
				for (auto& system : systems)
				{
					if (ImGui::TreeNodeEx(system->getName().c_str(), ImGuiTreeNodeFlags_CollapsingHeader))
					{
						system->draw();
					}
				}
			}
		}
		ImGui::End();
	}
}
