#include "editor.h"

#include "app/level_manager.h"
#include "core/renderer/rendering_device.h"
#include "core/resource_loader.h"
#include "core/input/input_manager.h"
#include "framework/components/hierarchy_component.h"
#include "framework/systems/render_system.h"
#include "framework/systems/physics_system.h"
#include "editor_application.h"
#include "main/window.h"

#include "imgui_stdlib.h"
#include "ImGuizmo.h"

void Editor::initialize(HWND hWnd, const JSON::json& projectJSON)
{
	BIND_EVENT_MEMBER_FUNCTION("EditorSaveBeforeQuit", Editor::saveBeforeQuit);
	BIND_EVENT_MEMBER_FUNCTION("EditorSaveAll", Editor::saveAll);
	BIND_EVENT_MEMBER_FUNCTION("EditorAutoSave", Editor::autoSave);
	BIND_EVENT_MEMBER_FUNCTION("EditorOpenLevel", Editor::openLevel);
	BIND_EVENT_MEMBER_FUNCTION("EditorCreateNewLevel", Editor::createNewLevel);
	BIND_EVENT_MEMBER_FUNCTION("EditorCreateNewEntity", Editor::createNewEntity);

	const JSON::json& general = projectJSON["general"];

	m_Colors.m_Accent = {
		(float)general["colors"]["accent"]["r"],
		(float)general["colors"]["accent"]["g"],
		(float)general["colors"]["accent"]["b"],
		(float)general["colors"]["accent"]["a"],
	};
	m_Colors.m_MediumAccent = {
		(float)general["colors"]["mediumAccent"]["r"],
		(float)general["colors"]["mediumAccent"]["g"],
		(float)general["colors"]["mediumAccent"]["b"],
		(float)general["colors"]["mediumAccent"]["a"],
	};
	m_Colors.m_HeavyAccent = {
		(float)general["colors"]["heavyAccent"]["r"],
		(float)general["colors"]["heavyAccent"]["g"],
		(float)general["colors"]["heavyAccent"]["b"],
		(float)general["colors"]["heavyAccent"]["a"],
	};
	m_Colors.m_Background = {
		(float)general["colors"]["background"]["r"],
		(float)general["colors"]["background"]["g"],
		(float)general["colors"]["background"]["b"],
		(float)general["colors"]["background"]["a"],
	};
	m_Colors.m_ItemBackground = {
		(float)general["colors"]["itemBackground"]["r"],
		(float)general["colors"]["itemBackground"]["g"],
		(float)general["colors"]["itemBackground"]["b"],
		(float)general["colors"]["itemBackground"]["a"],
	};
	m_Colors.m_Inactive = {
		(float)general["colors"]["inactive"]["r"],
		(float)general["colors"]["inactive"]["g"],
		(float)general["colors"]["inactive"]["b"],
		(float)general["colors"]["inactive"]["a"],
	};
	m_Colors.m_Success = {
		(float)general["colors"]["success"]["r"],
		(float)general["colors"]["success"]["g"],
		(float)general["colors"]["success"]["b"],
		(float)general["colors"]["success"]["a"],
	};
	m_Colors.m_Failure = {
		(float)general["colors"]["failure"]["r"],
		(float)general["colors"]["failure"]["g"],
		(float)general["colors"]["failure"]["b"],
		(float)general["colors"]["failure"]["a"],
	};
	m_Colors.m_FailAccent = {
		(float)general["colors"]["failAccent"]["r"],
		(float)general["colors"]["failAccent"]["g"],
		(float)general["colors"]["failAccent"]["b"],
		(float)general["colors"]["failAccent"]["a"],
	};
	m_Colors.m_Warning = {
		(float)general["colors"]["warning"]["r"],
		(float)general["colors"]["warning"]["g"],
		(float)general["colors"]["warning"]["b"],
		(float)general["colors"]["warning"]["a"],
	};
	m_Colors.m_Text = {
		(float)general["colors"]["text"]["r"],
		(float)general["colors"]["text"]["g"],
		(float)general["colors"]["text"]["b"],
		(float)general["colors"]["text"]["a"],
	};
	m_Colors.m_White = { 1.0f, 1.0f, 1.0f, 1.0f };

	m_FileSystem.reset(new FileSystemDock());
	m_Hierarchy.reset(new HierarchyDock());
	m_Output.reset(new OutputDock());
	m_Toolbar.reset(new ToolbarDock());
	m_Viewport.reset(new ViewportDock(projectJSON["viewport"]));
	m_Inspector.reset(new InspectorDock());
	m_FileViewer.reset(new FileViewer());

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigDockingWithShift = true;

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(RenderingDevice::GetSingleton()->getDevice(), RenderingDevice::GetSingleton()->getContext());
	ImGui::StyleColorsDark();

	registerEditorControls();
}

void Editor::render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	drawDefaultUI();
	m_FileSystem->draw();
	m_Hierarchy->draw();
	m_Toolbar->draw();
	m_Viewport->draw();
	m_Inspector->draw();
	m_FileViewer->draw();
	m_Output->draw();

	ImGui::PopStyleColor(m_EditorStyleColorPushCount);
	ImGui::PopStyleVar(m_EditorStyleVarPushCount);

	RenderingDevice::GetSingleton()->setTextureRenderTarget();
	if (m_WorldMode)
	{
		RenderSystem::GetSingleton()->render();
	}
	if (m_CollisionMode)
	{
		PhysicsSystem::GetSingleton()->debugDraw();
	}
	RenderingDevice::GetSingleton()->setBackBufferRenderTarget();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

Editor::~Editor()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Editor::drawDefaultUI()
{
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
			static String newLevelName;
			static String openLevelName;
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::BeginMenu("Create Entity", LevelManager::GetSingleton()->isAnyLevelOpen()))
				{
					for (auto&& entityClassFile : OS::GetFilesInDirectory("game/assets/classes/"))
					{
						if (ImGui::MenuItem(entityClassFile.string().c_str(), ""))
						{
							Variant callReturn = EventManager::GetSingleton()->returnCall("EditorFileCreateNewEntity", "EditorCreateNewEntity", entityClassFile.string());
							Ref<Entity> newEntity = Extract(Ref<Entity>, callReturn);
							EventManager::GetSingleton()->call("EditorFileOpenNewlyCreatedEntity", "EditorOpenEntity", newEntity);
						}
					}
					ImGui::EndMenu();
				}
				ImGui::Separator();
				if (ImGui::BeginMenu("Create Level"))
				{
					ImGui::InputText("Level Name", &newLevelName, ImGuiInputTextFlags_AlwaysInsertMode);
					ImGui::SameLine();
					if (ImGui::Button("Create"))
					{
						if (LevelManager::GetSingleton()->isAnyLevelOpen())
						{
							m_MenuAction = "Save";
							m_PopupCause = "create";
						}
						else
						{
							EventManager::GetSingleton()->call("EditorFileNewLevel", "EditorCreateNewLevel", newLevelName);
							EventManager::GetSingleton()->call("EditorOpenNewLevel", "EditorOpenLevel", "game/assets/levels/" + newLevelName);
						}
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Open Level"))
				{
					for (auto&& levelName : OS::GetDirectoriesInDirectory("game/assets/levels"))
					{
						if (ImGui::MenuItem(levelName.string().c_str()))
						{
							if (LevelManager::GetSingleton()->isAnyLevelOpen())
							{
								openLevelName = levelName.string();
								m_MenuAction = "Save";
								m_PopupCause = "open";
							}
							else
							{
								EventManager::GetSingleton()->call("EditorFileMenuOpenLevel", "EditorOpenLevel", levelName.string());
							}
						}
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Save Level", "", false, LevelManager::GetSingleton()->isAnyLevelOpen()))
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
			if (ImGui::BeginMenu("Assets"))
			{
				if (ImGui::MenuItem("Build Fonts"))
				{
					PRINT("Building fonts...");
					OS::Execute("start \"\" \"" + OS::GetAbsolutePath("build_fonts.bat").string() + "\"");
					PRINT("Built fonts");
				}
				if (ImGui::BeginMenu("Library"))
				{
					for (auto&& fileType : ResourceLoader::GetAllFiles())
					{
						for (auto&& file : fileType.second)
						{
							ImGui::SetNextItemWidth(file->getPath().string().size() * 8);
							ImGui::LabelText(std::to_string((int)file->getType()).c_str(), file->getPath().string().c_str());
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
				ImGui::Checkbox("World Mode", &m_WorldMode);

				bool fullscreen = Extract(bool, EventManager::GetSingleton()->returnCall("WindowGetScreenState", "WindowGetScreenState", 0));
				if (ImGui::Checkbox("Full Screen", &fullscreen))
				{
					EventManager::GetSingleton()->deferredCall("WindowToggleFullScreen", "WindowToggleFullScreen", 0);
				}
				if (ImGui::BeginMenu("Windows"))
				{
					ImGui::Checkbox("Toolbar", &m_Toolbar->getSettings().m_IsActive);
					ImGui::Checkbox("Output", &m_Output->getSettings().m_IsActive);
					ImGui::Checkbox("Hierarchy", &m_Hierarchy->getSettings().m_IsActive);
					ImGui::Checkbox("Entities", &m_Hierarchy->getSettings().m_IsEntitiesDockActive);
					ImGui::Checkbox("Viewport", &m_Viewport->getSettings().m_IsActive);
					ImGui::Checkbox("File System", &m_FileSystem->getSettings().m_IsActive);
					ImGui::Checkbox("Inspector", &m_Inspector->getSettings().m_IsActive);
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Level"))
			{
				if (ImGui::MenuItem("Settings"))
				{
					EventManager::GetSingleton()->call("EditorLevelMenu", "EditorOpenFile", LevelManager::GetSingleton()->getCurrentLevelSettingsFile()->getPath().string());
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About Rootex Editor"))
				{
					m_MenuAction = "About Rootex Editor";
				}
				if (ImGui::BeginMenu("Open Source Licenses"))
				{
					for (auto&& library : LevelManager::GetSingleton()->getLibrariesPaths())
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

			if (ImGui::BeginPopupModal("Save", 0, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::SetNextWindowSize({ ImGui::GetWindowWidth(), ImGui::GetWindowHeight() });
				ImGui::Text(String("Do you want to save " + LevelManager::GetSingleton()->getCurrentLevelName()+"?").c_str());
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
						EventManager::GetSingleton()->call("EditorFileNewLevel", "EditorCreateNewLevel", newLevelName);
						EventManager::GetSingleton()->call("EditorOpenNewLevel", "EditorOpenLevel", "game/assets/levels/" + newLevelName);
						ImGui::CloseCurrentPopup();
						m_MenuAction = "";
					}
					else if (m_PopupCause == "open")
					{
						saveAll(nullptr);
						EventManager::GetSingleton()->call("EditorFileMenuOpenLevel", "EditorOpenLevel", openLevelName);
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
						EventManager::GetSingleton()->call("EditorFileNewLevel", "EditorCreateNewLevel", newLevelName);
						EventManager::GetSingleton()->call("EditorOpenNewLevel", "EditorOpenLevel", "game/assets/levels/" + newLevelName);
						ImGui::CloseCurrentPopup();
						m_MenuAction = "";
					}
					else if (m_PopupCause == "open")
					{
						EventManager::GetSingleton()->call("EditorFileMenuOpenLevel", "EditorOpenLevel", openLevelName);
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

				static TextResourceFile* license = ResourceLoader::CreateLuaTextResourceFile("LICENSE");
				ImGui::Text(license->getData()->getRawData()->data());
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
			for (auto&& library : LevelManager::GetSingleton()->getLibrariesPaths())
			{
				if (ImGui::BeginPopup(library.string().c_str(), ImGuiWindowFlags_AlwaysAutoResize))
				{
					TextResourceFile* license = ResourceLoader::CreateLuaTextResourceFile(library.string() + "/LICENSE");
					ImGui::Text(license->getData()->getRawData()->data());
					m_MenuAction = "";
					ImGui::EndPopup();
				}
			}
			ImGui::EndMenuBar();
		}
	}
	ImGui::End();
}

void Editor::pushEditorStyleColors()
{
	// Every line in this function body should push a style color
	static const int starting = __LINE__;
	ImGui::PushStyleColor(ImGuiCol_DockingPreview, m_Colors.m_Accent);
	ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, m_Colors.m_Background);
	ImGui::PushStyleColor(ImGuiCol_Separator, m_Colors.m_HeavyAccent);
	ImGui::PushStyleColor(ImGuiCol_SeparatorActive, m_Colors.m_Accent);
	ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, m_Colors.m_MediumAccent);
	ImGui::PushStyleColor(ImGuiCol_FrameBg, m_Colors.m_ItemBackground);
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, m_Colors.m_MediumAccent);
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, m_Colors.m_HeavyAccent);
	ImGui::PushStyleColor(ImGuiCol_TitleBg, m_Colors.m_HeavyAccent);
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, m_Colors.m_Accent);
	ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, m_Colors.m_HeavyAccent);
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, m_Colors.m_HeavyAccent);
	ImGui::PushStyleColor(ImGuiCol_Header, m_Colors.m_HeavyAccent);
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, m_Colors.m_Success);
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, m_Colors.m_Accent);
	ImGui::PushStyleColor(ImGuiCol_PopupBg, m_Colors.m_Background);
	ImGui::PushStyleColor(ImGuiCol_Tab, m_Colors.m_HeavyAccent);
	ImGui::PushStyleColor(ImGuiCol_TabActive, m_Colors.m_Accent);
	ImGui::PushStyleColor(ImGuiCol_TabHovered, m_Colors.m_Accent);
	ImGui::PushStyleColor(ImGuiCol_TabUnfocused, m_Colors.m_MediumAccent);
	ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, m_Colors.m_Accent);
	ImGui::PushStyleColor(ImGuiCol_Border, m_Colors.m_MediumAccent);
	ImGui::PushStyleColor(ImGuiCol_BorderShadow, m_Colors.m_HeavyAccent);
	ImGui::PushStyleColor(ImGuiCol_Button, m_Colors.m_Success);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_Colors.m_Accent);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_Colors.m_Success);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, m_Colors.m_Accent);
	ImGui::PushStyleColor(ImGuiCol_Text, m_Colors.m_Text);
	ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, m_Colors.m_Accent);
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, m_Colors.m_Inactive);
	ImGui::PushStyleColor(ImGuiCol_ResizeGrip, m_Colors.m_HeavyAccent);
	ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, m_Colors.m_Accent);
	ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, m_Colors.m_MediumAccent);
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, m_Colors.m_HeavyAccent);
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, m_Colors.m_Accent);
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, m_Colors.m_MediumAccent);
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, m_Colors.m_Inactive);
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, m_Colors.m_Accent);
	static const int ending = m_EditorStyleColorPushCount = __LINE__ - starting - 1;
}

void Editor::pushEditorStyleVars()
{
	// Every line in this function body should push a style var
	static const int starting = __LINE__;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, { 0 });
	static const int ending = m_EditorStyleVarPushCount = __LINE__ - starting - 1;
}

void Editor::registerEditorControls()
{
	InputManager::GetSingleton()->mapBool("InputCameraActivate", Device::Mouse, MouseButton::MouseButtonRight);
	InputManager::GetSingleton()->mapBool("InputCameraForward", Device::Keyboard, KeyboardButton::KeyW);
	InputManager::GetSingleton()->mapBool("InputCameraLeft", Device::Keyboard, KeyboardButton::KeyA);
	InputManager::GetSingleton()->mapBool("InputCameraBackward", Device::Keyboard, KeyboardButton::KeyS);
	InputManager::GetSingleton()->mapBool("InputCameraRight", Device::Keyboard, KeyboardButton::KeyD);
	InputManager::GetSingleton()->mapBool("InputCameraUp", Device::Keyboard, KeyboardButton::KeySpace);
	InputManager::GetSingleton()->mapBool("InputCameraDown", Device::Keyboard, KeyboardButton::KeyC);

	InputManager::GetSingleton()->mapFloat("InputCameraTurnUp", Device::Mouse, MouseButton::MouseAxisY);
	InputManager::GetSingleton()->mapFloat("InputCameraTurnRight", Device::Mouse, MouseButton::MouseAxisX);
}

Variant Editor::saveAll(const Event* event)
{
	if (LevelManager::GetSingleton()->isAnyLevelOpen())
	{
		LevelManager::GetSingleton()->saveCurrentLevel();
		PRINT("Successfully saved level: " + LevelManager::GetSingleton()->getCurrentLevelName());
	}
	else
	{
		PRINT("No level is open. Did not save current level");
	}
	return true;
}

Variant Editor::autoSave(const Event* event)
{
	PRINT("Auto-saving entities...");
	saveAll(nullptr);
	return true;
}

Variant Editor::openLevel(const Event* event)
{
	String levelPath(Extract(String, event->getData()));
	LevelManager::GetSingleton()->openLevel(levelPath);
	SetWindowText(GetActiveWindow(), ("Rootex Editor: " + LevelManager::GetSingleton()->getCurrentLevelName()).c_str());

	return true;
}

Variant Editor::saveBeforeQuit(const Event* event)
{
	if (LevelManager::GetSingleton()->isAnyLevelOpen())
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

Variant Editor::createNewLevel(const Event* event)
{
	const String& newLevelName = Extract(String, event->getData());

	if (OS::IsExists("game/assets/levels/" + newLevelName))
	{
		WARN("Found a level with the same name: " + newLevelName);
		return true;
	}

	LevelManager::GetSingleton()->createLevel(newLevelName);

	return true;
}

Variant Editor::createNewEntity(const Event* event)
{
	const String& entityClassFilePath = Extract(String, event->getData());
	TextResourceFile* entityClassFile = ResourceLoader::CreateNewTextResourceFile(entityClassFilePath);

	Ref<Entity> newEntity = EntityFactory::GetSingleton()->createEntity(entityClassFile);

	HierarchySystem::GetSingleton()->addChild(newEntity);
	return newEntity;
}

Editor* Editor::GetSingleton()
{
	static Editor singleton;
	return &singleton;
}
