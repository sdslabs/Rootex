#include "toolbar_dock.h"

#include "editor/editor_application.h"

#include "app/level_manager.h"
#include "framework/components/script_component.h"
#include "framework/system.h"
#include "framework/systems/render_system.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_stdlib.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

#include "core/renderer/rendering_device.h"

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
				ImGui::PlotHistogram("FPS", m_FPSRecords.data(), m_FPSRecords.size(), 0, 0, 0.0f, 60.0f);
			}

			if (ImGui::TreeNodeEx("Events", ImGuiTreeNodeFlags_CollapsingHeader))
			{
				if (ImGui::Button("Add Event"))
				{
					ImGui::OpenPopup("Add Event");
				}

				if (ImGui::BeginPopupModal("Add Event", 0, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("Event Name");
					static String eventName;
					ImGui::InputText("##Event Name", &eventName);
					
					if (ImGui::Button("Create"))
					{
						EventManager::GetSingleton()->addEvent(eventName);
						PRINT("Added event: " + eventName);
						eventName = "";
					}
					ImGui::SameLine();
					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				for (auto&& [eventType, eventHandler] : EventManager::GetSingleton()->getRegisteredEvents())
				{
					ImGui::MenuItem(eventType.c_str(), "");
					ImGui::SameLine();
					ImGui::Text("%d", eventHandler.size());
				}
			}

			if (ImGui::TreeNodeEx("RenderSystem", ImGuiTreeNodeFlags_CollapsingHeader))
			{
				ImGui::Columns(2);

				ImGui::Text("Camera");
				ImGui::NextColumn();
				if (ImGui::BeginCombo("##Camera", RenderSystem::GetSingleton()->getCamera()->getOwner()->getFullName().c_str()))
				{
					for (auto&& camera : System::GetComponents(CameraComponent::s_ID))
					{
						if (ImGui::MenuItem(camera->getOwner()->getFullName().c_str()))
						{
							RenderSystem::GetSingleton()->setCamera((CameraComponent*)camera);
						}
					}

					ImGui::EndCombo();
				}

				ImGui::Columns(1);
			}

			if (ImGui::TreeNodeEx("UISystem", ImGuiTreeNodeFlags_CollapsingHeader))
			{
				ImGui::Columns(2);

				ImGui::Text("Debugger");
				ImGui::NextColumn();
				ImGui::Text("Press F8");
				ImGui::NextColumn();

				ImGui::Columns(1);
			}
		}
		ImGui::End();
	}
}
