#include "editor_system.h"

#include "core/random.h"
#include "core/renderer/rendering_device.h"
#include "core/resource_loader.h"
#include "core/resource_files/lua_text_resource_file.h"
#include "core/input/input_manager.h"
#include "framework/scene_loader.h"
#include "framework/systems/render_system.h"
#include "framework/systems/render_ui_system.h"
#include "framework/systems/script_system.h"
#include "framework/systems/ui_system.h"
#include "framework/systems/physics_system.h"
#include "editor_application.h"
#include "main/window.h"

#include "gui/scene_dock.h"
#include "gui/output_dock.h"
#include "gui/toolbar_dock.h"
#include "gui/viewport_dock.h"
#include "gui/inspector_dock.h"
#include "gui/file_viewer.h"
#include "gui/file_editor.h"
#include "gui/content_browser.h"

#include "imgui_stdlib.h"
#include "ImGuizmo.h"

#define DOCUMENTATION_LINK String("https://rootex.readthedocs.io/")

EditorSystem::EditorSystem()
    : System("EditorSystem", UpdateOrder::Editor, true)
    , m_CurrExportDir("")
    , m_IsCopyFailed(false)
{
	m_Binder.bind(EditorEvents::EditorSaveBeforeQuit, this, &EditorSystem::saveBeforeQuit);
	m_Binder.bind(EditorEvents::EditorSaveAll, this, &EditorSystem::saveAll);
	m_Binder.bind(EditorEvents::EditorAutoSave, this, &EditorSystem::autoSave);
	m_Binder.bind(EditorEvents::EditorCreateNewScene, this, &EditorSystem::createNewScene);
	m_Binder.bind(EditorEvents::EditorCreateNewFile, this, &EditorSystem::createNewFile);
}

bool EditorSystem::initialize(const JSON::json& systemData)
{
	m_Scene.reset(new SceneDock());
	m_Output.reset(new OutputDock());
	m_Toolbar.reset(new ToolbarDock());
	m_Viewport.reset(new ViewportDock(systemData["viewport"]));
	m_Inspector.reset(new InspectorDock());
	m_FileViewer.reset(new FileViewer());
	m_FileEditor.reset(new FileEditor());
	m_ContentBrowser.reset(new ContentBrowser());

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigDockingWithShift = true;
	io.FontAllowUserScaling = true;

	ImFontConfig fontConfig;
	fontConfig.PixelSnapH = true;
	fontConfig.OversampleH = 1;
	m_EditorFont = io.Fonts->AddFontFromFileTTF("editor/assets/fonts/DroidSans/DroidSans.ttf", 18.0f, &fontConfig);
	{
		static const ImWchar iconsRanges[] = { ICON_MIN_ROOTEX, ICON_MAX_ROOTEX, 0 };
		ImFontConfig iconsConfig;
		iconsConfig.MergeMode = true;
		iconsConfig.PixelSnapH = true;
		io.Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_ROOTEX, 18.0f, &iconsConfig, iconsRanges);
	}

	m_EditorFontItalic = io.Fonts->AddFontFromFileTTF("editor/assets/fonts/Cousine/Cousine-Italic.ttf", 18.0f, &fontConfig);
	{
		static const ImWchar iconsRanges[] = { ICON_MIN_ROOTEX, ICON_MAX_ROOTEX, 0 };
		ImFontConfig iconsConfig;
		iconsConfig.MergeMode = true;
		iconsConfig.PixelSnapH = true;
		io.Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_ROOTEX, 18.0f, &iconsConfig, iconsRanges);
	}
	m_EditorFontBold = io.Fonts->AddFontFromFileTTF("editor/assets/fonts/DroidSans/DroidSans-Bold.ttf", 18.0f, &fontConfig);
	{
		static const ImWchar iconsRanges[] = { ICON_MIN_ROOTEX, ICON_MAX_ROOTEX, 0 };
		ImFontConfig iconsConfig;
		iconsConfig.MergeMode = true;
		iconsConfig.PixelSnapH = true;
		io.Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_ROOTEX, 18.0f, &iconsConfig, iconsRanges);
	}
	m_EditorFontMonospace = io.Fonts->AddFontFromFileTTF("editor/assets/fonts/Cousine/Cousine-Regular.ttf", 18.0f, &fontConfig);
	{
		static const ImWchar iconsRanges[] = { ICON_MIN_ROOTEX, ICON_MAX_ROOTEX, 0 };
		ImFontConfig iconsConfig;
		iconsConfig.MergeMode = true;
		iconsConfig.PixelSnapH = true;
		io.Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_ROOTEX, 18.0f, &iconsConfig, iconsRanges);
	}
	ImGui_ImplWin32_Init(Application::GetSingleton()->getWindow()->getWindowHandle());
	ImGui_ImplDX11_Init(RenderingDevice::GetSingleton()->getDevice(), RenderingDevice::GetSingleton()->getContext());
	ImGui::StyleColorsDark();

	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(15, 15);
		style.WindowRounding = 5.0f;
		style.FramePadding = ImVec2(5, 5);
		style.FrameRounding = 4.0f;
		style.ItemSpacing = ImVec2(12, 6);
		style.ItemInnerSpacing = ImVec2(8, 4);
		style.IndentSpacing = 25.0f;
		style.ScrollbarSize = 15.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabMinSize = 5.0f;
		style.GrabRounding = 3.0f;

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.68f, 0.63f, 0.63f, 1.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.20f, 0.68f, 0.42f, 0.83f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.12f, 0.43f, 0.33f, 0.88f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.68f, 0.42f, 0.83f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.20f, 0.68f, 0.42f, 0.58f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.68f, 0.42f, 0.83f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.20f, 0.68f, 0.42f, 0.83f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.36f, 0.75f, 0.10f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.57f, 0.58f, 0.86f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.20f, 0.68f, 0.42f, 0.83f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.68f, 0.42f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.41f, 0.42f, 0.46f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.41f, 0.42f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.00f, 1.00f, 0.21f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.48f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
	}

	return true;
}

void EditorSystem::update(float deltaMilliseconds)
{
	ZoneScoped;
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	pushRegularFont();

	drawDefaultUI(deltaMilliseconds);
	m_Scene->draw(deltaMilliseconds);
	m_Toolbar->draw(deltaMilliseconds);
	m_Inspector->draw(deltaMilliseconds);
	m_FileViewer->draw(deltaMilliseconds);
	m_FileEditor->draw(deltaMilliseconds);
	m_Output->draw(deltaMilliseconds);
	m_Viewport->draw(deltaMilliseconds);
	m_ContentBrowser->draw(deltaMilliseconds);
	popFont();

	RenderingDevice::GetSingleton()->setMainRT();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	RenderingDevice::GetSingleton()->setOffScreenRTVDSV();
}

EditorSystem::~EditorSystem()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void EditorSystem::drawDefaultUI(float deltaMilliseconds)
{
	ZoneScoped;
	static bool optFullscreenPersistant = true;
	bool optFullscreen = optFullscreenPersistant;
	static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;
	if (optFullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 2.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		windowFlags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	static Scene* exportScene = nullptr;

	ImGui::Begin("Rootex Editor", nullptr, windowFlags);
	{
		ImGui::PopStyleVar();

		if (optFullscreen)
		{
			ImGui::PopStyleVar(2);
		}

		ImGuiID dockspaceId = ImGui::GetID("Rootex Editor Dockspace");
		ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

		if (ImGui::BeginMenuBar())
		{
			static String newSceneName;
			static String newFileName;
			static String newFileTypeName;
			static String newFileExtension;
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::BeginMenu("Create Resource"))
				{
					if (ImGui::BeginCombo("Resource Type", newFileTypeName.c_str()))
					{
						for (auto& [type, typeName] : ResourceFile::s_TypeNames)
						{
							if (const char* ext = ResourceLoader::GetCreatableExtension(type))
							{
								if (ImGui::Selectable(typeName.c_str()))
								{
									newFileExtension = ext;
									newFileTypeName = typeName;
								}
							}
						}
						ImGui::EndCombo();
					}
					if (!newFileTypeName.empty())
					{
						ImGui::InputText("Resource Name", &newFileName);

						String finalNewFileName = "game/assets/materials/" + newFileName + newFileExtension;
						ImGui::Text("File Name: %s", finalNewFileName.c_str());

						if (!newFileName.empty()
						    && newFileName.find('.') == String::npos
						    && ImGui::Button("Create"))
						{
							EventManager::GetSingleton()->call(EditorEvents::EditorCreateNewFile, finalNewFileName);
						}
					}

					ImGui::EndMenu();
				}
				static String newScript;
				if (ImGui::BeginMenu("Create Script"))
				{
					ImGui::InputText("Script Name", &newScript);

					String finalNewScriptName = "game/assets/scripts/" + newScript + ".lua";
					ImGui::Text("File Name: %s", finalNewScriptName.c_str());

					if (!newScript.empty() && ImGui::Button("Create"))
					{
						if (!OS::IsExists(finalNewScriptName))
						{
							InputOutputFileStream file = OS::CreateFileName(finalNewScriptName);
							String defaultScript = ResourceLoader::CreateLuaTextResourceFile("rootex/assets/scripts/empty.lua")->getString();
							file.write(defaultScript.c_str(), strlen(defaultScript.c_str()));
							file.close();
							PRINT("Successfully created script: " + finalNewScriptName);
							newScript = "";
						}
						else
						{
							WARN("Script already exists: " + finalNewScriptName);
						}
					}
					ImGui::EndMenu();
				}
				ImGui::Separator();
				if (ImGui::BeginMenu("Create Scene"))
				{
					ImGui::InputText("Scene Name", &newSceneName, ImGuiInputTextFlags_AlwaysInsertMode);
					if (!newSceneName.empty() && ImGui::Button("Create"))
					{
						if (SceneLoader::GetSingleton()->getCurrentScene())
						{
							m_MenuAction = "Save";
							m_PopupCause = "create";
						}
						else
						{
							EventManager::GetSingleton()->call(EditorEvents::EditorCreateNewScene, newSceneName);
							loadingScene = "game/assets/scenes/" + newSceneName + ".scene.json";
						}
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Open Scene"))
				{
					for (auto&& levelName : OS::GetFilesInDirectory("game/assets/scenes/"))
					{
						if (ImGui::MenuItem(levelName.generic_string().c_str()))
						{
							openScene(levelName.generic_string());
						}
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Save Scene", "", false, (bool)SceneLoader::GetSingleton()->getCurrentScene()))
				{
					EventManager::GetSingleton()->call(EditorEvents::EditorSaveAll);
				}
				if (OS::GetBuildType() == "Release")
				{
					if (ImGui::MenuItem("Export Scene", "", false, (bool)SceneLoader::GetSingleton()->getCurrentScene()))
					{
						exportScene = SceneLoader::GetSingleton()->getCurrentScene();
					}
				}
				if (ImGui::MenuItem("Preferences"))
				{
					EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { ApplicationSettings::GetSingleton()->getTextFile()->getPath().generic_string(), (int)ApplicationSettings::GetSingleton()->getTextFile()->getType() });
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Quit", ""))
				{
					EventManager::GetSingleton()->call(EditorEvents::EditorSaveBeforeQuit, 0);
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Assets"))
			{
				if (ImGui::MenuItem("Build Fonts"))
				{
					PRINT("Building fonts...");
					OS::Execute("start \"\" \"" + OS::GetAbsolutePath("build_fonts.bat").string() + "\"");
					PRINT("Built fonts");
				}
				if (ImGui::BeginMenu("Resources"))
				{
					int id = 0;
					for (auto& [type, files] : ResourceLoader::GetResources())
					{
						for (auto& fileHandle : files)
						{
							Ref<ResourceFile> file = fileHandle.lock();
							if (!file)
							{
								continue;
							}

							ImGui::PushID(id);
							if (file->isDirty())
							{
								if (ImGui::Button(ICON_ROOTEX_REFRESH "##Refresh Asset"))
								{
									file->reimport();
								}
								ImGui::SameLine();
								ImGui::TextColored(getWarningColor(), "%s", file->getPath().generic_string().c_str());
							}
							else
							{
								ImGui::Text(file->getPath().generic_string().c_str());
							}
							ImGui::SameLine();
							ImGui::BulletText("%s", ResourceFile::s_TypeNames.at(file->getType()).c_str());
							ImGui::PopID();
							id++;
						}
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			static bool styleEditor = false;
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::Checkbox("Wireframe Mode", &m_WireframeMode))
				{
					if (m_WireframeMode)
					{
						RenderSystem::GetSingleton()->enableWireframeRasterizer();
					}
					else
					{
						RenderSystem::GetSingleton()->resetDefaultRasterizer();
					}
				}

				ImGui::Checkbox("Style Editor", &styleEditor);

				bool fullscreen = Extract<bool>(EventManager::GetSingleton()->returnCall(RootexEvents::WindowGetScreenState));
				if (ImGui::Checkbox("Full Screen", &fullscreen))
				{
					EventManager::GetSingleton()->deferredCall(RootexEvents::WindowToggleFullscreen);
				}
				if (ImGui::BeginMenu("Windows"))
				{
					ImGui::Checkbox("Toolbar", &m_Toolbar->getSettings().m_IsActive);
					ImGui::Checkbox("Output", &m_Output->getSettings().m_IsActive);
					ImGui::Checkbox("Scene", &m_Scene->getSettings().m_IsActive);
					ImGui::Checkbox("Viewport", &m_Viewport->getSettings().m_IsActive);
					ImGui::Checkbox("Inspector", &m_Inspector->getSettings().m_IsActive);
					ImGui::Checkbox("Content Browser", &m_ContentBrowser->getSettings().m_IsActive);
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Scene"))
			{
				ImGui::Checkbox("Autosave", &m_Autosave);

				if (SceneLoader::GetSingleton()->getCurrentScene())
				{
					SceneLoader::GetSingleton()->getCurrentScene()->getSettings().draw();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Documentation"))
			{
				if (ImGui::BeginMenu("Guides"))
				{
					static Vector<FilePath> guidesFiles = OS::GetAllFilesInDirectory("docs/guides/");
					for (auto& file : guidesFiles)
					{
						if (ImGui::MenuItem(file.generic_string().c_str()))
						{
							EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { file.generic_string(), (int)ResourceFile::Type::Text });
						}
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Engine"))
				{
					static Vector<FilePath> engineDocFiles = OS::GetAllFilesInDirectory("docs/engine/");
					for (auto& file : engineDocFiles)
					{
						if (ImGui::MenuItem(file.generic_string().c_str()))
						{
							EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { file.generic_string(), (int)ResourceFile::Type::Text });
						}
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Engine API"))
				{
					static Vector<FilePath> apiFiles = OS::GetAllFilesInDirectory("docs/api/");
					for (auto& file : apiFiles)
					{
						if (ImGui::MenuItem(file.generic_string().c_str()))
						{
							EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { file.generic_string(), (int)ResourceFile::Type::Text });
						}
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Lua API"))
				{
					m_MenuAction = "Lua API Documentation";
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Online Documentation " ICON_ROOTEX_EXTERNAL_LINK))
				{
					OS::Execute("start " + DOCUMENTATION_LINK);
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("About"))
			{
				if (ImGui::MenuItem("About Rootex Editor"))
				{
					m_MenuAction = "About Rootex Editor";
				}
				if (ImGui::BeginMenu("Open Source Licenses"))
				{
					for (auto&& library : Application::GetSingleton()->getLibrariesPaths())
					{
						if (ImGui::MenuItem(library.filename().string().c_str(), ""))
						{
							m_MenuAction = library.string();
						}
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}

			if (styleEditor)
			{
				ImGui::Begin("Style Editor");
				ImGui::ShowStyleEditor();
				ImGui::End();
			}

			if (m_MenuAction != "")
			{
				ImGui::OpenPopup(m_MenuAction.c_str());
			}

			ImGui::SetNextWindowSize({ ImGui::GetContentRegionMax().x / 2, ImGui::GetContentRegionMax().y / 2 });
			if (ImGui::BeginPopupModal("Lua API Documentation", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				static String searchingString;
				ImGui::InputText("Search", &searchingString, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AlwaysInsertMode | ImGuiInputTextFlags_CallbackHistory);
				ImGui::SameLine();
				if (ImGui::Button("Close"))
				{
					ImGui::CloseCurrentPopup();
					m_MenuAction = "";
				}

				static String openedLuaClass;
				if (ImGui::ListBoxHeader("##Lua Classes", { 0, ImGui::GetContentRegionAvail().y }))
				{
					for (const auto& [key, value] : LuaInterpreter::GetSingleton()->getLuaState().registry())
					{
						if (key.is<String>())
						{
							sol::object luaClass = value;
							String typeName = key.as<String>();

							bool shouldMatch = false;
							for (int i = 0; i < typeName.size(); i++)
							{
								shouldMatch |= typeName.compare(i, searchingString.size(), searchingString) == 0;
							}

							if (searchingString.empty() || shouldMatch)
							{
								if (ImGui::MenuItem(typeName.c_str()))
								{
									openedLuaClass = typeName;
								}
							}
						}
					}
					ImGui::ListBoxFooter();
				}
				ImGui::SameLine();
				if (ImGui::ListBoxHeader("##Class Description", ImGui::GetContentRegionAvail()))
				{
					if (!openedLuaClass.empty())
					{
						sol::object luaClass = LuaInterpreter::GetSingleton()->getLuaState().registry()[openedLuaClass];

						EditorSystem::GetSingleton()->pushBoldFont();
						ImGui::Text("%s", openedLuaClass.c_str());
						EditorSystem::GetSingleton()->popFont();

						ImGui::SetNextItemOpen(true);
						showDocumentation(openedLuaClass, luaClass);
					}
					ImGui::ListBoxFooter();
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginPopupModal("Save", 0, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::SetNextWindowSize({ ImGui::GetWindowWidth(), ImGui::GetWindowHeight() });
				ImGui::Text(String("Do you want to save " + SceneLoader::GetSingleton()->getCurrentScene()->getFullName() + "?").c_str());
				if (ImGui::Button("Save"))
				{
					if (m_PopupCause == "quit")
					{
						saveAll(nullptr);
						EventManager::GetSingleton()->call(RootexEvents::QuitEditorWindow);
					}
					else if (m_PopupCause == "create")
					{
						saveAll(nullptr);
						EventManager::GetSingleton()->call(EditorEvents::EditorCreateNewScene, newSceneName);
						loadingScene = "game/assets/scenes/" + newSceneName + ".scene.json";
						ImGui::CloseCurrentPopup();
						m_MenuAction = "";
					}
					else if (m_PopupCause == "open")
					{
						saveAll(nullptr);
						loadingScene = openSceneName;
						ImGui::CloseCurrentPopup();
						m_MenuAction = "";
					}
				}
				ImGui::SameLine();

				if (ImGui::Button("Don't Save"))
				{
					if (m_PopupCause == "quit")
					{
						EventManager::GetSingleton()->call(RootexEvents::QuitEditorWindow);
					}
					else if (m_PopupCause == "create")
					{
						EventManager::GetSingleton()->call(EditorEvents::EditorCreateNewScene, newSceneName);
						loadingScene = "game/assets/scenes/" + newSceneName + ".scene.json";
						ImGui::CloseCurrentPopup();
						m_MenuAction = "";
					}
					else if (m_PopupCause == "open")
					{
						loadingScene = openSceneName;
						ImGui::CloseCurrentPopup();
						m_MenuAction = "";
					}
				}
				ImGui::SameLine();

				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
					m_MenuAction = "";
				}
				ImGui::EndPopup();
			}

			if (ImGui::BeginPopup("About Rootex Editor"))
			{
				static Ref<ImageResourceFile> engineImage = ResourceLoader::CreateImageResourceFile("rootex/assets/rootex.png");
				static Ref<ImageResourceFile> editorImage = ResourceLoader::CreateImageResourceFile("editor/assets/editor.png");

				ImGui::BeginGroup();
				ImGui::Image(engineImage->getGPUTexture()->getTextureResourceView(), { 200, 200 });
				ImGui::Text("Rootex Engine");
				ImGui::EndGroup();

				ImGui::SameLine();

				ImGui::BeginGroup();
				ImGui::Image(editorImage->getGPUTexture()->getTextureResourceView(), { 200, 200 });
				ImGui::Text("Rootex Editor");
				ImGui::EndGroup();

				ImGui::Text(String("Rootex Engine and Rootex Editor developed by SDSLabs. Built on " + OS::GetBuildDate() + " at " + OS::GetBuildTime() + "\n" + "Source available at https://www.github.com/sdslabs/rootex").c_str());

				static Ref<TextResourceFile> license = ResourceLoader::CreateTextResourceFile("LICENSE");
				ImGui::Text("%s", license->getString().c_str());
				m_MenuAction = "";
				ImGui::EndPopup();
			}
			if (ImGui::BeginPopup("Proprietary Licenses"))
			{
				ImGui::Text("To be added");
				ImGui::EndPopup();
			}

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
			for (auto&& library : Application::GetSingleton()->getLibrariesPaths())
			{
				if (ImGui::BeginPopup(library.string().c_str(), ImGuiWindowFlags_AlwaysAutoResize))
				{
					Ref<TextResourceFile> license = ResourceLoader::CreateTextResourceFile(library.string() + "/LICENSE");
					ImGui::Text("%s", license->getString().c_str());
					m_MenuAction = "";
					ImGui::EndPopup();
				}
			}
			ImGui::EndMenuBar();
		}
	}

	static Atomic<int> loadingSceneProgress = 0;
	static float loadingSceneCurrentProgress = 0.0f;
	static int loadingSceneTotalProgress = -1;

	if (!loadingScene.empty() && loadingSceneTotalProgress == -1)
	{
		ImGui::OpenPopup("Load Scene");
		loadingSceneTotalProgress = SceneLoader::GetSingleton()->preloadScene(loadingScene, loadingSceneProgress);
		loadingSceneCurrentProgress = 0.0f;
	}

	if (ImGui::BeginPopupModal("Load Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Loading scene: %s", loadingScene.c_str());

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());

		drawProgressBar(loadingSceneProgress, loadingSceneCurrentProgress, loadingSceneTotalProgress);

		if (loadingSceneTotalProgress == loadingSceneProgress)
		{
			EventManager::GetSingleton()->call(EditorEvents::EditorSceneIsClosing);
			SceneLoader::GetSingleton()->loadPreloadedScene(loadingScene, {});
			SetWindowText(GetActiveWindow(), ("Rootex Editor: " + loadingScene).c_str());
			loadingSceneTotalProgress = -1;
			loadingSceneProgress = 0;
			loadingScene = "";
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	static Atomic<int> exportSceneProgress = 0;
	static float exportSceneCurrentProgress = 0.0f;
	static int exportSceneTotalProgress = -1;

	if (exportScene && exportSceneTotalProgress == -1)
	{
		ImGui::OpenPopup("Export Scene");
		exportSceneTotalProgress = EditorSystem::exportScene(exportScene->getName(), exportScene->getScenePath(), exportSceneProgress);
		exportSceneCurrentProgress = 0.0f;
	}

	if (ImGui::BeginPopupModal("Export Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Export scene: %s", exportScene->getName().c_str());

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());

		drawProgressBar(exportSceneProgress, exportSceneCurrentProgress, exportSceneTotalProgress);

		if (exportSceneTotalProgress == exportSceneProgress)
		{
			exportSceneTotalProgress = -1;
			exportSceneProgress = 0;
			exportScene = nullptr;
			ImGui::CloseCurrentPopup();
			postExport();
		}

		ImGui::EndPopup();
	}

	ImGui::End();
}

static HashMap<int, String> LuaTypeNames = {
	{ LUA_TNONE, "none" },
	{ LUA_TNIL, "lua_nil" },
	{ LUA_TSTRING, "string" },
	{ LUA_TNUMBER, "number" },
	{ LUA_TTHREAD, "thread" },
	{ LUA_TBOOLEAN, "boolean" },
	{ LUA_TFUNCTION, "function" },
	{ LUA_TUSERDATA, "userdata" },
	{ LUA_TLIGHTUSERDATA, "lightuserdata" },
	{ LUA_TTABLE, "table" },
	{ -0xFFFF, "poly" }
};

void EditorSystem::showDocumentation(const String& name, const sol::table& table)
{
	if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Indent();
		for (auto [key, value] : table)
		{
			if (key.is<String>() && key.as<String>() != name && key.as<String>() != "class_check" && key.as<String>() != "class_cast")
			{
				if (value.is<sol::table>())
				{
					showDocumentation(name + "." + key.as<String>(), value.as<sol::table>());
				}
				else
				{
					String tag = LuaTypeNames[(int)value.get_type()];
					String prefix = name;
					String suffix;
					if (value.is<sol::function>())
					{
						suffix = "()";
					}
					else if (value.is<String>())
					{
						suffix = " = " + value.as<String>();
					}

					pushItalicFont();
					ImGui::TextColored(getSuccessColor(), "%s", tag.c_str());
					ImGui::SameLine();
					ImGui::TextColored(getWarningColor(), "%s", prefix.c_str());
					popFont();

					ImGui::SameLine();

					pushBoldFont();
					ImGui::MenuItem((" . " + key.as<String>() + suffix).c_str());
					popFont();
				}
			}
		}
		ImGui::Unindent();
	}
}

void EditorSystem::openScene(String sceneName)
{
	if (SceneLoader::GetSingleton()->getCurrentScene())
	{
		openSceneName = sceneName;
		m_MenuAction = "Save";
		m_PopupCause = "open";
	}
	else
	{
		loadingScene = sceneName;
	}
}

Variant EditorSystem::saveAll(const Event* event)
{
	if (SceneLoader::GetSingleton()->getCurrentScene())
	{
		for (auto& [fileType, fileExt] : CreatableFiles)
		{
			ResourceLoader::SaveResources(fileType);
		}
		SceneLoader::GetSingleton()->saveScene(SceneLoader::GetSingleton()->getCurrentScene());
		PRINT("Successfully saved current scene: " + SceneLoader::GetSingleton()->getCurrentScene()->getFullName());
	}
	else
	{
		PRINT("No level is open. Did not save current scene");
	}
	return true;
}

Variant EditorSystem::autoSave(const Event* event)
{
	if (m_Autosave && !(m_Toolbar->getSettings().m_InEditorPlaying))
	{
		PRINT("Auto-saving current scene...");
		saveAll(nullptr);
		return true;
	}

	return false;
}

Variant EditorSystem::saveBeforeQuit(const Event* event)
{
	if (SceneLoader::GetSingleton()->getCurrentScene())
	{
		m_MenuAction = "Save";
		m_PopupCause = "quit";
	}
	else
	{
		EventManager::GetSingleton()->call(EditorEvents::EditorSceneIsClosing);
		EventManager::GetSingleton()->call(RootexEvents::QuitEditorWindow);
	}
	return true;
}

Variant EditorSystem::createNewScene(const Event* event)
{
	const String& sceneName = Extract<String>(event->getData());
	const String& newScenePath = "game/assets/scenes/" + sceneName + ".scene.json";
	if (OS::IsExists(newScenePath))
	{
		WARN("Found a scene with the same typeName: " + newScenePath);
		return true;
	}

	Ref<Scene> newScene = Scene::CreateEmptyAtPath(newScenePath);
	newScene->setName(sceneName);
	SceneLoader::GetSingleton()->saveScene(newScene.get());
	return true;
}

Variant EditorSystem::createNewFile(const Event* event)
{
	OS::CreateFileName(Extract<String>(event->getData()));
	return true;
}

int EditorSystem::exportScene(const String& sceneName, const String& sceneFilePath, Atomic<int>& progress)
{
	progress = 0;

	JSON::json exportTemplate = JSON::json::parse(ResourceLoader::CreateTextResourceFile("editor/export.template.json")->getString());

	Vector<Pair<String, String>> toCopy;

	for (auto& paths : exportTemplate["fileMappings"].items())
	{
		toCopy.push_back({ paths.key(), paths.value() });
	}

	int i = 0;
	do
	{
		m_CurrExportDir = "exports/" + sceneName;
		if (i != 0)
		{
			m_CurrExportDir += std::to_string(i);
		}
		m_CurrExportDir += "/";
		i++;
	} while (OS::IsExists(m_CurrExportDir));

	OS::CreateDirectoryName(m_CurrExportDir);
	OS::CreateDirectoryName(m_CurrExportDir + "game/assets/");
	OS::CreateDirectoryName(m_CurrExportDir + "rootex/assets/");

	JSON::json gameConfig = JSON::json::parse(ResourceLoader::CreateTextResourceFile("game/game.app.json")->getString());
	gameConfig["startScene"] = sceneFilePath;
	Ref<TextResourceFile> newGameConfig = ResourceLoader::CreateNewTextResourceFile(m_CurrExportDir + "game/game.app.json");
	newGameConfig->putString(gameConfig.dump(4));

	if (!newGameConfig->save())
	{
		WARN("Could not save application settings file");
		OS::DeleteDirectory(m_CurrExportDir);
		return 0;
	}

	Ref<TextResourceFile> readme = ResourceLoader::CreateNewTextResourceFile(m_CurrExportDir + "readme.txt");
	readme->putString("This Game was built using Rootex Game Engine. Find the source code here http://github.com/SDSLabs/Rootex.");
	if (!readme->save())
	{
		WARN("Could not save readme file");
		OS::DeleteDirectory(m_CurrExportDir);
		return 0;
	}

	Vector<FilePath> assetFiles = OS::GetAllFilesInDirectory("game/assets/");
	Vector<FilePath> rootexAssetFiles = OS::GetAllFilesInDirectory("rootex/assets/");
	Vector<FilePath> shaderFiles = OS::GetAllFilesInDirectory("rootex/core/renderer/shaders");
	assetFiles.insert(assetFiles.end(), rootexAssetFiles.begin(), rootexAssetFiles.end());
	assetFiles.insert(assetFiles.end(), shaderFiles.begin(), shaderFiles.end());

	for (auto& file : assetFiles)
	{
		toCopy.push_back({ file.generic_string(), file.generic_string() });
	}

	Vector<Ref<Task>> tasks;
	m_IsCopyFailed = false;

	for (auto& filePair : toCopy)
	{
		tasks.push_back(std::make_shared<Task>([=, &progress]() {
			progress++;
			if (m_IsCopyFailed)
			{
				return;
			}
			m_IsCopyFailed = !OS::RelativeCopyFile(filePair.first, m_CurrExportDir + filePair.second);
		}));
	}

	/// TODO: Fix the need for this dummy task (blocks the main thread while tasks are running)
	tasks.push_back(std::make_shared<Task>([]() {}));
	progress++;

	ThreadPool& threadPool = Application::GetSingleton()->getThreadPool();
	threadPool.submit(tasks);

	PRINT("Exporting to " + m_CurrExportDir)

	return tasks.size();
}

void EditorSystem::postExport()
{
	if (m_IsCopyFailed)
	{
		PRINT("Export failed");
	}
	else
	{
		PRINT("Successfully exported to " + m_CurrExportDir);
	}
	m_CurrExportDir = "";
	m_IsCopyFailed = false;
}

void EditorSystem::drawProgressBar(Atomic<int>& progress, float& currentProgress, int& totalProgress)
{
	float target = progress.load() / (float)totalProgress;
	float velocity = (target - currentProgress) * Random::Float();
	currentProgress += velocity * 0.01f;
	ImGui::ProgressBar(currentProgress);
}

EditorSystem* EditorSystem::GetSingleton()
{
	static EditorSystem singleton;
	return &singleton;
}

ImColor ColorToImColor(Color& c)
{
	return ImColor(c.x, c.y, c.z, c.w);
}
