#include "toolbar_dock.h"

#include "editor/editor_application.h"

#include "app/project_manager.h"
#include "framework/components/script_component.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_stdlib.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

#include "core/renderer/rendering_device.h"

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
				OS::Execute("\"" + OS::GetGameExecutablePath() + "\" " + ProjectManager::GetSingleton()->getCurrentLevelName());
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
				float currentTime = EditorApplication::GetSingleton()->getAppFrameTimer().getTimeMs();
				ImGui::LabelText(std::to_string(currentTime - m_LastUpdateTime).c_str(), "Frame Time");
				ImGui::LabelText(std::to_string((unsigned int)(1.0f / ((currentTime - m_LastUpdateTime) * MS_TO_S))).c_str(), "Frames Per Second");
				m_LastUpdateTime = currentTime;
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
					ImGui::BeginGroup();
					ImGui::MenuItem(eventType.c_str(), "");
					ImGui::SameLine();
					ImGui::Text("%d", eventHandler.size());
					ImGui::EndGroup();

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ConnectEvent"))
						{
							Pair<ScriptComponent*, String>* script = (Pair<ScriptComponent*, String>*)payload->Data;
							script->first->connect(script->second, eventType);
						}
						ImGui::EndDragDropTarget();
					}
				}
			}
		}
		ImGui::End();
	}
}
