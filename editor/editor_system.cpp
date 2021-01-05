#include "editor_system.h"

#include "core/random.h"
#include "core/renderer/rendering_device.h"
#include "core/renderer/material_library.h"
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

#include "imgui_stdlib.h"
#include "ImGuizmo.h"
#include "ImGuiFileDialog.h"

#define DOCUMENTATION_LINK String("https://rootex.readthedocs.io/en/latest/api/rootex.html")

Vector<String> Split(const String& s, char delim)
{
	Vector<String> elems;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

bool EditorSystem::initialize(const JSON::json& systemData)
{
	BIND_EVENT_MEMBER_FUNCTION("EditorSaveBeforeQuit", EditorSystem::saveBeforeQuit);
	BIND_EVENT_MEMBER_FUNCTION("EditorSaveAll", EditorSystem::saveAll);
	BIND_EVENT_MEMBER_FUNCTION("EditorAutoSave", EditorSystem::autoSave);
	BIND_EVENT_MEMBER_FUNCTION("EditorCreateNewScene", EditorSystem::createNewScene);
	BIND_EVENT_MEMBER_FUNCTION("EditorCreateNewMaterial", EditorSystem::createNewMaterial);

	const JSON::json& general = systemData["general"];

	m_Colors.accent = {
		(float)general["colors"]["accent"]["r"],
		(float)general["colors"]["accent"]["g"],
		(float)general["colors"]["accent"]["b"],
		(float)general["colors"]["accent"]["a"],
	};
	m_Colors.mediumAccent = {
		(float)general["colors"]["mediumAccent"]["r"],
		(float)general["colors"]["mediumAccent"]["g"],
		(float)general["colors"]["mediumAccent"]["b"],
		(float)general["colors"]["mediumAccent"]["a"],
	};
	m_Colors.heavyAccent = {
		(float)general["colors"]["heavyAccent"]["r"],
		(float)general["colors"]["heavyAccent"]["g"],
		(float)general["colors"]["heavyAccent"]["b"],
		(float)general["colors"]["heavyAccent"]["a"],
	};
	m_Colors.background = {
		(float)general["colors"]["background"]["r"],
		(float)general["colors"]["background"]["g"],
		(float)general["colors"]["background"]["b"],
		(float)general["colors"]["background"]["a"],
	};
	m_Colors.itemBackground = {
		(float)general["colors"]["itemBackground"]["r"],
		(float)general["colors"]["itemBackground"]["g"],
		(float)general["colors"]["itemBackground"]["b"],
		(float)general["colors"]["itemBackground"]["a"],
	};
	m_Colors.inactive = {
		(float)general["colors"]["inactive"]["r"],
		(float)general["colors"]["inactive"]["g"],
		(float)general["colors"]["inactive"]["b"],
		(float)general["colors"]["inactive"]["a"],
	};
	m_Colors.success = {
		(float)general["colors"]["success"]["r"],
		(float)general["colors"]["success"]["g"],
		(float)general["colors"]["success"]["b"],
		(float)general["colors"]["success"]["a"],
	};
	m_Colors.failure = {
		(float)general["colors"]["failure"]["r"],
		(float)general["colors"]["failure"]["g"],
		(float)general["colors"]["failure"]["b"],
		(float)general["colors"]["failure"]["a"],
	};
	m_Colors.failAccent = {
		(float)general["colors"]["failAccent"]["r"],
		(float)general["colors"]["failAccent"]["g"],
		(float)general["colors"]["failAccent"]["b"],
		(float)general["colors"]["failAccent"]["a"],
	};
	m_Colors.warning = {
		(float)general["colors"]["warning"]["r"],
		(float)general["colors"]["warning"]["g"],
		(float)general["colors"]["warning"]["b"],
		(float)general["colors"]["warning"]["a"],
	};
	m_Colors.text = {
		(float)general["colors"]["text"]["r"],
		(float)general["colors"]["text"]["g"],
		(float)general["colors"]["text"]["b"],
		(float)general["colors"]["text"]["a"],
	};
	m_Colors.white = { 1.0f, 1.0f, 1.0f, 1.0f };

	m_Scene.reset(new SceneDock());
	m_Output.reset(new OutputDock());
	m_Toolbar.reset(new ToolbarDock());
	m_Viewport.reset(new ViewportDock(systemData["viewport"]));
	m_Inspector.reset(new InspectorDock());
	m_FileViewer.reset(new FileViewer());

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_DpiEnableScaleFonts | ImGuiConfigFlags_DpiEnableScaleViewports;
	io.ConfigDockingWithShift = true;
	io.FontAllowUserScaling = true;
	
	ImFontConfig fontConfig;
	fontConfig.PixelSnapH = true;
	fontConfig.OversampleH = 1;
	m_EditorFont = io.Fonts->AddFontFromFileTTF("editor/assets/fonts/Lato-Regular.ttf", 18.0f, &fontConfig);
	
	static const ImWchar iconsRanges[] = { ICON_MIN_ROOTEX, ICON_MAX_ROOTEX, 0 };
	ImFontConfig iconsConfig;
	iconsConfig.MergeMode = true;
	iconsConfig.PixelSnapH = true;
	io.Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_ROOTEX, 18.0f, &iconsConfig, iconsRanges);
	
	m_EditorFontItalic = io.Fonts->AddFontFromFileTTF("editor/assets/fonts/Lato-Italic.ttf", 18.0f, &fontConfig);
	m_EditorFontBold = io.Fonts->AddFontFromFileTTF("editor/assets/fonts/Lato-Bold.ttf", 18.0f, &fontConfig);
	
	ImGui_ImplWin32_Init(Application::GetSingleton()->getWindow()->getWindowHandle());
	ImGui_ImplDX11_Init(RenderingDevice::GetSingleton()->getDevice(), RenderingDevice::GetSingleton()->getContext());
	ImGui::StyleColorsDark();

	for (auto& extension : Split(SupportedFiles.at(ResourceFile::Type::Model), ','))
	{
		igfd::ImGuiFileDialog::Instance()->SetExtentionInfos(
		    extension, { general["colors"]["files"]["model"]["r"], general["colors"]["files"]["model"]["g"], general["colors"]["files"]["model"]["b"], general["colors"]["files"]["model"]["a"] }, m_Icons.model);	
	}
	for (auto& extension : Split(SupportedFiles.at(ResourceFile::Type::Image), ','))
	{
		igfd::ImGuiFileDialog::Instance()->SetExtentionInfos(
		    extension, { general["colors"]["files"]["image"]["r"], general["colors"]["files"]["image"]["g"], general["colors"]["files"]["image"]["b"], general["colors"]["files"]["image"]["a"] }, m_Icons.image);
	}
	for (auto& extension : Split(SupportedFiles.at(ResourceFile::Type::Audio), ','))
	{
		igfd::ImGuiFileDialog::Instance()->SetExtentionInfos(
		    extension, { general["colors"]["files"]["audio"]["r"], general["colors"]["files"]["audio"]["g"], general["colors"]["files"]["audio"]["b"], general["colors"]["files"]["audio"]["a"] }, m_Icons.audio);
	}
	for (auto& extension : Split(SupportedFiles.at(ResourceFile::Type::Text), ','))
	{
		igfd::ImGuiFileDialog::Instance()->SetExtentionInfos(
		    extension, { general["colors"]["files"]["text"]["r"], general["colors"]["files"]["text"]["g"], general["colors"]["files"]["text"]["b"], general["colors"]["files"]["text"]["a"] }, m_Icons.text);
	}
	for (auto& extension : Split(SupportedFiles.at(ResourceFile::Type::Lua), ','))
	{
		igfd::ImGuiFileDialog::Instance()->SetExtentionInfos(
		    extension, { general["colors"]["files"]["lua"]["r"], general["colors"]["files"]["lua"]["g"], general["colors"]["files"]["lua"]["b"], general["colors"]["files"]["lua"]["a"] }, m_Icons.lua);
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
	m_Viewport->draw(deltaMilliseconds);
	m_Inspector->draw(deltaMilliseconds);
	m_FileViewer->draw(deltaMilliseconds);
	m_Output->draw(deltaMilliseconds);

	ImGui::PopStyleColor(m_EditorStyleColorPushCount);
	ImGui::PopStyleVar(m_EditorStyleVarPushCount);

	popFont();

	if (m_CollisionMode)
	{
		PhysicsSystem::GetSingleton()->debugDraw();
	}
	
	RenderingDevice::GetSingleton()->setMainRT();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	RenderingDevice::GetSingleton()->setOffScreenRT();
}

void EditorSystem::pushRegularFont()
{
	ImGui::PushFont(m_EditorFont);
}

void EditorSystem::pushBoldFont()
{
	ImGui::PushFont(m_EditorFontBold);
}

void EditorSystem::pushItalicFont()
{
	ImGui::PushFont(m_EditorFontItalic);
}

void EditorSystem::popFont()
{
	ImGui::PopFont();
}

EditorSystem::EditorSystem()
    : System("EditorSystem", UpdateOrder::Editor, true)
{
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

		pushEditorStyleColors();
		pushEditorStyleVars();

		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		windowFlags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	
	static String loadingScene;

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
			static String openSceneName;
			static String newMaterialName;
			static String newMaterialType;
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::BeginMenu("Create Material"))
				{
					if (ImGui::BeginCombo("Material Type", newMaterialType.c_str()))
					{
						for (auto& [materialType, materialCreators] : MaterialLibrary::GetMaterialDatabase())
						{
							if (ImGui::Selectable(materialType.c_str(), ""))
							{
								newMaterialType = materialType;
							}
						}
						ImGui::EndCombo();
					}
					ImGui::InputText("Material Name", &newMaterialName, ImGuiInputTextFlags_AlwaysInsertMode);
					if (!newMaterialName.empty() && !newMaterialType.empty() && ImGui::Button("Create"))
					{
						Vector<String> newMaterialInfo = { "game/assets/materials/" + newMaterialName + ".rmat", newMaterialType };
						EventManager::GetSingleton()->call("EditorFileCreateNewMaterial", "EditorCreateNewMaterial", newMaterialInfo);
					}

					ImGui::EndMenu();
				}
				static String newScript;
				if (ImGui::BeginMenu("Create Script"))
				{
					ImGui::InputText("Script Name", &newScript, ImGuiInputTextFlags_AlwaysInsertMode);
					if (!newScript.empty() && ImGui::Button("Create"))
					{
						newScript = "game/assets/scripts/" + newScript + ".lua";
						if (!OS::IsExists(newScript))
						{
							InputOutputFileStream file = OS::CreateFileName(newScript);
							String defaultScript = ResourceLoader::CreateLuaTextResourceFile("rootex/assets/scripts/empty.lua")->getString();
							file.write(defaultScript.c_str(), strlen(defaultScript.c_str()));
							file.close();
							PRINT("Successfully created script: " + newScript);
							newScript = "";
						}
						else
						{
							WARN("Script already exists: " + newScript);
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
							EventManager::GetSingleton()->call("EditorFileNewLevel", "EditorCreateNewScene", newSceneName);
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
							if (SceneLoader::GetSingleton()->getCurrentScene())
							{
								openSceneName = levelName.generic_string();
								m_MenuAction = "Save";
								m_PopupCause = "open";
							}
							else
							{
								loadingScene = levelName.generic_string();
							}
						}
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Instantiate Scene", 0, false, (bool)SceneLoader::GetSingleton()->getCurrentScene()))
				{
					igfd::ImGuiFileDialog::Instance()->OpenModal("ChooseSceneFile", "Choose Scene File", ".json", "game/assets/");
				}
				if (ImGui::MenuItem("Save Scene", "", false, (bool)SceneLoader::GetSingleton()->getCurrentScene()))
				{
					EventManager::GetSingleton()->call("EditorSaveEvent", "EditorSaveAll", 0);
				}
				if (ImGui::MenuItem("Preferences"))
				{
					EventManager::GetSingleton()->call("EditorSettingsMenu", "EditorOpenFile", ApplicationSettings::GetSingleton()->getTextFile()->getPath().string());
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Quit", ""))
				{
					EventManager::GetSingleton()->call("EditorSaveBeforeQuit", "EditorSaveBeforeQuit", 0);
				}

				ImGui::EndMenu();
			}
			if (igfd::ImGuiFileDialog::Instance()->FileDialog("ChooseSceneFile"))
			{
				if (igfd::ImGuiFileDialog::Instance()->IsOk)
				{
					FilePath filePath = OS::GetRootRelativePath(igfd::ImGuiFileDialog::Instance()->GetFilePathName());
					SceneLoader::GetSingleton()->getCurrentScene()->addChild(Scene::CreateFromFile(filePath.generic_string()));
				}
				igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseSceneFile");
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
					for (auto& [type, files] : ResourceLoader::GetResources())
					{
						for (auto& file : files)
						{
							ImGui::MenuItem(file->getPath().generic_string().c_str());
						}
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
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

				ImGui::Checkbox("Collision Mode", &m_CollisionMode);
				
				bool fullscreen = Extract<bool>(EventManager::GetSingleton()->returnCall("WindowGetScreenState", "WindowGetScreenState", 0));
				if (ImGui::Checkbox("Full Screen", &fullscreen))
				{
					EventManager::GetSingleton()->deferredCall("WindowToggleFullScreen", "WindowToggleFullScreen", 0);
				}
				if (ImGui::BeginMenu("Windows"))
				{
					ImGui::Checkbox("Toolbar", &m_Toolbar->getSettings().m_IsActive);
					ImGui::Checkbox("Output", &m_Output->getSettings().m_IsActive);
					ImGui::Checkbox("Hierarchy", &m_Scene->getSettings().m_IsActive);
					ImGui::Checkbox("Entities", &m_Scene->getSettings().m_IsEntitiesDockActive);
					ImGui::Checkbox("Viewport", &m_Viewport->getSettings().m_IsActive);
					ImGui::Checkbox("Inspector", &m_Inspector->getSettings().m_IsActive);
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Scene"))
			{
				if (SceneLoader::GetSingleton()->getCurrentScene())
				{
					SceneLoader::GetSingleton()->getCurrentScene()->getSettings().draw();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Documentation"))
			{
				if (ImGui::BeginMenu("C++ API"))
				{
					if (ImGui::MenuItem("Open Online C++ Documentation"))
					{
						OS::Execute("start " + DOCUMENTATION_LINK);
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Lua API"))
				{
					m_MenuAction = "Lua API Documentation";
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
					for (auto& [key, value] : LuaInterpreter::GetSingleton()->getLuaState().registry())
					{
						if (key.is<String>())
						{
							sol::object luaClass = value;
							String typeName = key.as<String>();
							if (luaClass.is<sol::table>() && luaClass.as<sol::table>()["__type"] != sol::nil)
							{
								typeName = luaClass.as<sol::table>()["__type"]["name"];
							}

							bool shouldMatch = false;
							for (int i = 0; i < typeName.size(); i++)
							{
								shouldMatch |= typeName.compare(i, searchingString.size(), searchingString) == 0;
							}

							if (searchingString.empty() || shouldMatch)
							{
								if (ImGui::MenuItem(typeName.c_str()))
								{
									openedLuaClass = key.as<String>();
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
						String typeName = openedLuaClass;
						if (luaClass.is<sol::table>() && luaClass.as<sol::table>()["__type"] != sol::nil)
						{
							typeName = luaClass.as<sol::table>()["__type"]["name"];
						}
						
						EditorSystem::GetSingleton()->pushBoldFont();
						ImGui::Text("%s", typeName.c_str());
						EditorSystem::GetSingleton()->popFont();

						ImGui::SetNextItemOpen(true);
						showDocumentation(typeName, luaClass);
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
						EventManager::GetSingleton()->call("QuitEditorWindow", "QuitEditorWindow", 0);
					}
					else if (m_PopupCause == "create")
					{
						saveAll(nullptr);
						EventManager::GetSingleton()->call("EditorFileNewScene", "EditorCreateNewScene", newSceneName);
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
						EventManager::GetSingleton()->call("QuitEditorWindow", "QuitEditorWindow", 0);
					}
					else if (m_PopupCause == "create")
					{
						EventManager::GetSingleton()->call("EditorFileNewLevel", "EditorCreateNewScene", newSceneName);
						loadingScene = newSceneName;
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
				ImGui::Text(String("Rootex Engine and Rootex Editor developed by SDSLabs. Built on " + OS::GetBuildDate() + " at " + OS::GetBuildTime() + "\n" + "Source available at https://www.github.com/sdslabs/rootex").c_str());

				static TextResourceFile* license = ResourceLoader::CreateTextResourceFile("LICENSE");
				ImGui::Text("%s", license->getString().c_str());
				ImGui::Separator();
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
					TextResourceFile* license = ResourceLoader::CreateTextResourceFile(library.string() + "/LICENSE");
					ImGui::Text("%s", license->getString().c_str());
					m_MenuAction = "";
					ImGui::EndPopup();
				}
			}
			ImGui::EndMenuBar();
		}
	}

	static Atomic<int> progress;
	static float currentProgress = 0.0f;
	static int totalProgress = -1;

	if (!loadingScene.empty() && totalProgress == -1)
	{
		ImGui::OpenPopup("Load Scene");
		totalProgress = SceneLoader::GetSingleton()->preloadScene(loadingScene, progress);
		currentProgress = 0.0f;
	}

	if (ImGui::BeginPopupModal("Load Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Loading scene: %s", loadingScene.c_str());

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
		float target = progress.load() / (float)totalProgress;
		float velocity = (target - currentProgress) * Random::Float();
		currentProgress += velocity * 0.01f;
		ImGui::ProgressBar(currentProgress);

		if (totalProgress == progress)
		{
			EventManager::GetSingleton()->call("EditorLoadScene", "EditorCloseScene", 0);
			SceneLoader::GetSingleton()->loadPreloadedScene(loadingScene, {});
			SetWindowText(GetActiveWindow(), ("Rootex Editor: " + loadingScene).c_str());
			totalProgress = -1;
			progress = 0;
			loadingScene = "";
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::End();
}

void EditorSystem::pushEditorStyleColors()
{
	// Every line in this function body should push a style color
	static const int starting = __LINE__;
	ImGui::PushStyleColor(ImGuiCol_DockingPreview, m_Colors.accent);
	ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, m_Colors.background);
	ImGui::PushStyleColor(ImGuiCol_Separator, m_Colors.heavyAccent);
	ImGui::PushStyleColor(ImGuiCol_SeparatorActive, m_Colors.accent);
	ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, m_Colors.mediumAccent);
	ImGui::PushStyleColor(ImGuiCol_FrameBg, m_Colors.itemBackground);
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, m_Colors.mediumAccent);
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, m_Colors.heavyAccent);
	ImGui::PushStyleColor(ImGuiCol_TitleBg, m_Colors.heavyAccent);
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, m_Colors.accent);
	ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, m_Colors.heavyAccent);
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, m_Colors.heavyAccent);
	ImGui::PushStyleColor(ImGuiCol_Header, m_Colors.heavyAccent);
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, m_Colors.success);
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, m_Colors.accent);
	ImGui::PushStyleColor(ImGuiCol_PopupBg, m_Colors.background);
	ImGui::PushStyleColor(ImGuiCol_Tab, m_Colors.heavyAccent);
	ImGui::PushStyleColor(ImGuiCol_TabActive, m_Colors.accent);
	ImGui::PushStyleColor(ImGuiCol_TabHovered, m_Colors.accent);
	ImGui::PushStyleColor(ImGuiCol_TabUnfocused, m_Colors.mediumAccent);
	ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, m_Colors.accent);
	ImGui::PushStyleColor(ImGuiCol_Border, m_Colors.mediumAccent);
	ImGui::PushStyleColor(ImGuiCol_BorderShadow, m_Colors.heavyAccent);
	ImGui::PushStyleColor(ImGuiCol_Button, m_Colors.success);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_Colors.accent);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_Colors.success);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, m_Colors.accent);
	ImGui::PushStyleColor(ImGuiCol_Text, m_Colors.text);
	ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, m_Colors.accent);
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, m_Colors.inactive);
	ImGui::PushStyleColor(ImGuiCol_ResizeGrip, m_Colors.heavyAccent);
	ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, m_Colors.accent);
	ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, m_Colors.mediumAccent);
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, m_Colors.heavyAccent);
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, m_Colors.accent);
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, m_Colors.mediumAccent);
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, m_Colors.inactive);
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, m_Colors.accent);
	static const int ending = m_EditorStyleColorPushCount = __LINE__ - starting - 1;
}

void EditorSystem::pushEditorStyleVars()
{
	// Every line in this function body should push a style var
	static const int starting = __LINE__;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, { 0 });
	static const int ending = m_EditorStyleVarPushCount = __LINE__ - starting - 1;
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
					ImGui::TextColored(getColors().success, "%s", tag.c_str());
					ImGui::SameLine();
					ImGui::TextColored(getColors().warning, "%s", prefix.c_str());
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

Variant EditorSystem::saveAll(const Event* event)
{
	if (SceneLoader::GetSingleton()->getCurrentScene())
	{
		MaterialLibrary::SaveAll();
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
	PRINT("Auto-saving current scene...");
	saveAll(nullptr);
	return true;
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
		EventManager::GetSingleton()->call("QuitEditorWindow", "QuitEditorWindow", 0);
	}
	return true;
}

Variant EditorSystem::createNewScene(const Event* event)
{
	const String& sceneName = Extract<String>(event->getData());
	const String& newScenePath = "game/assets/scenes/" + sceneName + ".scene.json";
	if (OS::IsExists(newScenePath))
	{
		WARN("Found a level with the same name: " + newScenePath);
		return true;
	}

	Ref<Scene> newScene = Scene::CreateEmptyAtPath(newScenePath);
	newScene->setName(sceneName);
	SceneLoader::GetSingleton()->saveScene(newScene.get());
	return true;
}

Variant EditorSystem::createNewMaterial(const Event* event)
{
	const Vector<String>& materialInfo = Extract<Vector<String>>(event->getData());
	MaterialLibrary::CreateNewMaterialFile(materialInfo[0], materialInfo[1]);
	return true;
}

EditorSystem* EditorSystem::GetSingleton()
{
	static EditorSystem singleton;
	return &singleton;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void EditorSystem::draw()
{
	System::draw();

	if (!m_IsActive)
	{
		m_IsActive = true;
		WARN("Disabling the editor will freeze everything");
	}
}
#endif // ROOTEX_EDITOR
