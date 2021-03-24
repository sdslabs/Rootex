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
					OS::RunApplication("\"" + OS::GetGameExecutablePath() + "\" " + SceneLoader::GetSingleton()->getCurrentScene()->getScenePath());
					PRINT("Launched Game process");
				}
			}
			ImGui::NextColumn();

			ImGui::Text("Play Game");
			ImGui::SameLine();
			if (ImGui::ArrowButton("Play Game", ImGuiDir_Right))
			{
				EventManager::GetSingleton()->call(EditorEvents::EditorSaveAll);
				OS::RunApplication("\"" + OS::GetGameExecutablePath() + "\"");
				PRINT("Launched Game process");
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

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
			ImGui::SliderFloat("##Delta Multiplier", Application::GetSingleton()->getDeltaMultiplierPtr(), -2.0f, 2.0f, "");
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				Application::GetSingleton()->resetDeltaMultiplier();
			}

			if (ImGui::TreeNodeEx("Editor"))
			{
				RootexFPSGraph("FPS", m_FPSRecords, EditorApplication::GetSingleton()->getAppFrameTimer().getLastFPS());
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
