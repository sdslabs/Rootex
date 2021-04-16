#include "toolbar_dock.h"

#include "editor/editor_application.h"

#include "framework/scene_loader.h"
#include "framework/system.h"
#include "editor/editor_system.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_stdlib.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

Variant ToolbarDock::disablePlayInEditor(const Event* e)
{
	m_InEditorPlaying = false;
	EditorApplication::GetSingleton()->setGameMode(false);
	if (!m_StartPlayingScene.empty())
	{
		SceneLoader::GetSingleton()->loadScene(m_StartPlayingScene, {});
		m_StartPlayingScene.clear();
	}
	PRINT("Stopped Scene in Editor");
	return true;
}

ToolbarDock::ToolbarDock()
    : m_Binder(this)
{
	m_Binder.bind(EditorEvents::EditorSceneIsClosing, &ToolbarDock::disablePlayInEditor);
	m_Binder.bind(RootexEvents::QuitEditorWindow, &ToolbarDock::disablePlayInEditor);

	m_FPSRecords.resize(m_FPSRecordsPoolSize, 0.0f);
}

void ToolbarDock::draw(float deltaMilliseconds)
{
	ZoneScoped;
	if (m_ToolbarDockSettings.m_IsActive)
	{
		if (ImGui::Begin("Toolbar"))
		{
			if (SceneLoader::GetSingleton()->getCurrentScene())
			{
				static int playModeSelected = 0;
				static const char* playModes[3] = {
					"Play Scene in Editor",
					"Play Scene in Game",
					"Play Game"
				};

				if (m_InEditorPlaying)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)(EditorSystem::GetSingleton()->getFatalColor()));
					if (ImGui::Button("Stop " ICON_ROOTEX_WINDOW_CLOSE, { ImGui::GetContentRegionAvailWidth(), 40.0f }))
					{
						disablePlayInEditor(nullptr);
					}
					ImGui::PopStyleColor();
				}
				else
				{
					if (ImGui::Button("Play " ICON_ROOTEX_FORT_AWESOME, { ImGui::GetContentRegionAvailWidth(), 40.0f }))
					{
						switch (playModeSelected)
						{
						case 0:
							m_InEditorPlaying = true;
							m_StartPlayingScene = SceneLoader::GetSingleton()->getCurrentScene()->getScenePath();
							EventManager::GetSingleton()->call(EditorEvents::EditorSaveAll);
							EditorApplication::GetSingleton()->setGameMode(true);
							SceneLoader::GetSingleton()->loadScene(m_StartPlayingScene, {});
							PRINT("Loaded Scene in Editor");
							break;
						case 1:
							m_InEditorPlaying = false;
							EventManager::GetSingleton()->call(EditorEvents::EditorSaveAll);
							OS::RunApplication("\"" + OS::GetGameExecutablePath() + "\" " + SceneLoader::GetSingleton()->getCurrentScene()->getScenePath());
							PRINT("Launched Game process with Scene");
							break;
						case 2:
							m_InEditorPlaying = false;
							EventManager::GetSingleton()->call(EditorEvents::EditorSaveAll);
							OS::RunApplication("\"" + OS::GetGameExecutablePath() + "\"");
							PRINT("Launched Game process");
							break;
						default:
							WARN("Unknown play mode found");
						}
					}
				}

				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
				if (ImGui::BeginCombo("##Play Mode", playModes[playModeSelected]))
				{
					for (int i = 0; i < sizeof(playModes) / sizeof(playModes[0]); i++)
					{
						if (ImGui::MenuItem(playModes[i]))
						{
							playModeSelected = i;
						}
					}
					ImGui::EndCombo();
				}
			}

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
