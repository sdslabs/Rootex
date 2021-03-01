#include "toolbar_dock.h"

#include "editor/editor_application.h"

#include "framework/scene_loader.h"
#include "framework/system.h"
#include "editor/editor_system.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_stdlib.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

ToolbarDock::ToolbarDock()
{
	m_FPSRecords.resize(m_FPSRecordsPoolSize, 0.0f);
}

void ToolbarDock::draw(float deltaMilliseconds)
{
	ZoneScoped;
	if (m_ToolbarDockSettings.m_IsActive)
	{
		if (ImGui::Begin("Toolbar"))
		{
			ImGui::Columns(2);

			if (SceneLoader::GetSingleton()->getCurrentScene())
			{
				ImGui::Text("Play Scene");
				ImGui::SameLine();
				if (ImGui::ArrowButton("Play Scene", ImGuiDir_Right))
				{
					EventManager::GetSingleton()->call(EditorEvents::EditorSaveAll);
					PRINT("Launched Game process");
					OS::Execute("\"" + OS::GetGameExecutablePath() + "\" " + SceneLoader::GetSingleton()->getCurrentScene()->getSceneFilePath());
					PRINT("Game process ended");
				}
			}
			ImGui::NextColumn();

			ImGui::Text("Play Game");
			ImGui::SameLine();
			if (ImGui::ArrowButton("Play Game", ImGuiDir_Right))
			{
				EventManager::GetSingleton()->call(EditorEvents::EditorSaveAll);
				PRINT("Launched Game process");
				OS::Execute("\"" + OS::GetGameExecutablePath() + "\"");
				PRINT("Game process ended");
			}
			ImGui::NextColumn();

#ifdef TRACY_ENABLE
			ImGui::Text("Profiler");
			ImGui::NextColumn();
			if (ImGui::Button("Start Tracy " ICON_ROOTEX_EXTERNAL_LINK " "))
			{
				OS::OpenFileInSystemEditor("rootex/vendor/Tracy/Tracy.exe");
			}
			ImGui::NextColumn();
#endif // TRACY_ENABLE

			ImGui::Columns(1);

			if (ImGui::TreeNodeEx("Editor"))
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
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Events"))
			{
				for (auto&& [eventType, eventHandlers] : EventManager::GetSingleton()->getRegisteredEvents())
				{
					ImGui::Text((eventType + " (" + std::to_string(eventHandlers.size()) + ")").c_str());
				}
				ImGui::TreePop();
			}

			for (auto& systems : System::GetSystems())
			{
				for (auto& system : systems)
				{
					if (ImGui::TreeNodeEx(system->getName().c_str()))
					{
						system->draw();
						ImGui::TreePop();
					}
				}
			}
		}
		ImGui::End();
	}
}
