#include "editor.h"

#include "core/renderer/rendering_device.h"
#include "core/resource_loader.h"

#include "framework/components/hierarchy_component.h"
#include "framework/systems/render_system.h"

void Editor::initialize(HWND hWnd)
{
	LuaVariable general = LuaInterpreter::GetSingleton()->getGlobal("general");

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
	m_Colors.m_Inactive = {
		(float)general["colors"]["inactive"]["r"],
		(float)general["colors"]["inactive"]["g"],
		(float)general["colors"]["inactive"]["b"],
		(float)general["colors"]["inactive"]["a"],
	};

	m_FileSystem.reset(new FileSystemDock());
	m_Hierarchy.reset(new HierarchyDock());
	m_Output.reset(new OutputDock());
	m_Toolbar.reset(new ToolbarDock());
	m_Viewport.reset(new ViewportDock(LuaInterpreter::GetSingleton()->getGlobal("viewport")));
	m_Inspector.reset(new InspectorDock());

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigDockingWithShift = true;

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(RenderingDevice::GetSingleton()->getDevice(), RenderingDevice::GetSingleton()->getContext());
	ImGui::StyleColorsDark();
}

void Editor::render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	drawDefaultUI();
	m_FileSystem->draw();
	m_Hierarchy->draw();
	m_Output->draw();
	m_Toolbar->draw();
	m_Viewport->draw();
	m_Inspector->draw();

	ImGui::PopStyleColor(m_EditorStyleColorPushCount);
	ImGui::PopStyleVar(m_EditorStyleVarPushCount);

	RenderingDevice::GetSingleton()->setTextureRenderTarget();
	RenderSystem::GetSingleton()->render();
	RenderingDevice::GetSingleton()->setBackBufferRenderTarget();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Editor::quit()
{
	PostQuitMessage(0);
}

void Editor::log(const String& log)
{
	m_Output->log(log);
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
			static String menuAction = "";
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Lua File", ""))
				{
				}
				ImGui::MenuItem("Save");
				ImGui::MenuItem("Save All", "");
				ImGui::Separator();
				if (ImGui::MenuItem("Quit", ""))
				{
					quit();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::BeginMenu("Windows"))
				{
					ImGui::Checkbox("Toolbar", &m_Toolbar->getSettings().m_IsActive);
					ImGui::Checkbox("Output", &m_Output->getSettings().m_IsActive);
					ImGui::Checkbox("Hierarchy", &m_Hierarchy->getSettings().m_IsActive);
					ImGui::Checkbox("Viewport", &m_Viewport->getSettings().m_IsActive);
					ImGui::Checkbox("File System", &m_FileSystem->getSettings().m_IsActive);
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About Rootex Editor"))
				{
					menuAction = "About Rootex Editor";
				}
				if (ImGui::MenuItem("Open Source Licenses"))
				{
					menuAction = "Open Source Licenses";
				}
				ImGui::EndMenu();
			}

			if (menuAction != "")
			{
				ImGui::OpenPopup(menuAction.c_str());
			}

			if (ImGui::BeginPopup("About Rootex Editor"))
			{
				ImGui::Text(String("Rootex Engine and Rootex Editor developed by SDSLabs. Built on " + OS::GetBuildDate() + " at " + OS::GetBuildTime() + "\n" + "Source available at https://www.github.com/sdslabs/rootex").c_str());

				static TextResourceFile* license = ResourceLoader::CreateLuaTextResourceFile("LICENSE");
				ImGui::Text(license->getData()->getRawData()->data());
				ImGui::Separator();
				menuAction = "";
				ImGui::EndPopup();
			}
			if (ImGui::BeginPopup("Proprietary Licenses"))
			{
				ImGui::Text("To be added");
				ImGui::EndPopup();
			}
			if (ImGui::BeginPopup("Open Source Licenses"))
			{
				ImGui::BulletText("Bullet3D");
				ImGui::NewLine();
				static TextResourceFile* bulletLicense = ResourceLoader::CreateLuaTextResourceFile("rootex/vendor/Bullet3D/LICENSE.txt");
				ImGui::Text(bulletLicense->getData()->getRawData()->data());
				ImGui::Separator();

				ImGui::BulletText("DirectXTK");
				ImGui::NewLine();
				static TextResourceFile* directXTKLicense = ResourceLoader::CreateLuaTextResourceFile("rootex/vendor/DirectXTK/LICENSE");
				ImGui::Text(directXTKLicense->getData()->getRawData()->data());
				ImGui::Separator();

				ImGui::BulletText("Gainput");
				ImGui::NewLine();
				static TextResourceFile* gainputLicense = ResourceLoader::CreateLuaTextResourceFile("rootex/vendor/Gainput/LICENSE");
				ImGui::Text(gainputLicense->getData()->getRawData()->data());
				ImGui::Separator();

				ImGui::BulletText("Dear ImGui");
				ImGui::NewLine();
				static TextResourceFile* imGuiLicense = ResourceLoader::CreateLuaTextResourceFile("rootex/vendor/ImGUI/LICENSE.txt");
				ImGui::Text(imGuiLicense->getData()->getRawData()->data());
				ImGui::Separator();

				ImGui::BulletText("Lua C API");
				ImGui::NewLine();
				static TextResourceFile* luaCAPILicense = ResourceLoader::CreateLuaTextResourceFile("rootex/vendor/Lua/LICENSE.txt");
				ImGui::Text(luaCAPILicense->getData()->getRawData()->data());
				ImGui::Separator();

				ImGui::BulletText("LuaBridge");
				ImGui::NewLine();
				static TextResourceFile* luaBridgeLicense = ResourceLoader::CreateLuaTextResourceFile("rootex/vendor/LuaBridge/LICENSE.txt");
				ImGui::Text(luaBridgeLicense->getData()->getRawData()->data());
				ImGui::Separator();

				ImGui::BulletText("OBJLoader");
				ImGui::NewLine();
				static TextResourceFile* objLoaderLicense = ResourceLoader::CreateLuaTextResourceFile("rootex/vendor/OBJLoader/License.txt");
				ImGui::Text(objLoaderLicense->getData()->getRawData()->data());
				ImGui::Separator();

				ImGui::BulletText("ALUT");
				ImGui::NewLine();
				static TextResourceFile* alutLicense = ResourceLoader::CreateLuaTextResourceFile("rootex/vendor/OpenAL/COPYING.txt");
				ImGui::Text(alutLicense->getData()->getRawData()->data());
				ImGui::Separator();

				menuAction = "";
				ImGui::EndPopup();
			}
			ImGui::EndMenuBar();
		}
	}
	ImGui::End();
}

void Editor::pushEditorStyleColors()
{
	m_EditorStyleColorPushCount = 0;

	ImGui::PushStyleColor(ImGuiCol_DockingPreview, m_Colors.m_Accent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, m_Colors.m_MediumAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_Separator, m_Colors.m_HeavyAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_SeparatorActive, m_Colors.m_Accent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, m_Colors.m_MediumAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_BorderShadow, m_Colors.m_HeavyAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_FrameBg, m_Colors.m_HeavyAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, m_Colors.m_MediumAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, m_Colors.m_HeavyAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_TitleBg, m_Colors.m_HeavyAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, m_Colors.m_Accent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, m_Colors.m_HeavyAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, m_Colors.m_HeavyAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_Header, m_Colors.m_HeavyAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, m_Colors.m_MediumAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, m_Colors.m_Accent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_PopupBg, m_Colors.m_Inactive);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_Tab, m_Colors.m_HeavyAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_TabActive, m_Colors.m_Accent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_TabHovered, m_Colors.m_Accent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_TabUnfocused, m_Colors.m_MediumAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, m_Colors.m_Accent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_Border, m_Colors.m_HeavyAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_Button, m_Colors.m_Accent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_Button, m_Colors.m_Accent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_Colors.m_Accent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_Colors.m_Accent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_CheckMark, m_Colors.m_Accent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, m_Colors.m_Accent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, m_Colors.m_Inactive);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_ResizeGrip, m_Colors.m_HeavyAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, m_Colors.m_Accent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, m_Colors.m_MediumAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, m_Colors.m_HeavyAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, m_Colors.m_Accent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, m_Colors.m_MediumAccent);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, m_Colors.m_Inactive);
	m_EditorStyleColorPushCount++;
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, m_Colors.m_Accent);
	m_EditorStyleColorPushCount++;
}

void Editor::pushEditorStyleVars()
{
	m_EditorStyleVarPushCount = 0;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, { 0 });
	m_EditorStyleVarPushCount++;
}

Editor* Editor::GetSingleton()
{
	static Editor singleton;
	return &singleton;
}

void Editor::logToOutput(const String& log)
{
	m_Output->log(log);
}
