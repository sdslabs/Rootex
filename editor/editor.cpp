#include "editor.h"

#include "core/renderer/rendering_device.h"
#include "core/resource_loader.h"

#include "framework/components/visual/visual_component_graph.h"
#include "framework/components/hierarchy_component.h"
#include "framework/systems/render_system.h"

void Editor::initialize(HWND hWnd)
{
	BIND_MEMBER("EditorQuit", Editor::quit);

	m_EditorConfig.loadExecuteScript(ResourceLoader::CreateLuaTextResourceFile("editor/config/editor_config.lua"));

	m_Colors.m_Accent = {
		(float)m_EditorConfig.getGlobal("general")["colors"]["accent"]["r"],
		(float)m_EditorConfig.getGlobal("general")["colors"]["accent"]["g"],
		(float)m_EditorConfig.getGlobal("general")["colors"]["accent"]["b"],
		(float)m_EditorConfig.getGlobal("general")["colors"]["accent"]["a"],
	};
	m_Colors.m_MediumAccent = {
		(float)m_EditorConfig.getGlobal("general")["colors"]["mediumAccent"]["r"],
		(float)m_EditorConfig.getGlobal("general")["colors"]["mediumAccent"]["g"],
		(float)m_EditorConfig.getGlobal("general")["colors"]["mediumAccent"]["b"],
		(float)m_EditorConfig.getGlobal("general")["colors"]["mediumAccent"]["a"],
	};
	m_Colors.m_HeavyAccent = {
		(float)m_EditorConfig.getGlobal("general")["colors"]["heavyAccent"]["r"],
		(float)m_EditorConfig.getGlobal("general")["colors"]["heavyAccent"]["g"],
		(float)m_EditorConfig.getGlobal("general")["colors"]["heavyAccent"]["b"],
		(float)m_EditorConfig.getGlobal("general")["colors"]["heavyAccent"]["a"],
	};
	m_Colors.m_Inactive = {
		(float)m_EditorConfig.getGlobal("general")["colors"]["inactive"]["r"],
		(float)m_EditorConfig.getGlobal("general")["colors"]["inactive"]["g"],
		(float)m_EditorConfig.getGlobal("general")["colors"]["inactive"]["b"],
		(float)m_EditorConfig.getGlobal("general")["colors"]["inactive"]["a"],
	};

	m_ViewportSettings.m_AspectRatio = (float)m_EditorConfig.getGlobal("viewport")["aspectRatio"];
	m_ViewportSettings.m_ImageTint = {
		(float)m_EditorConfig.getGlobal("viewport")["imageTint"]["r"],
		(float)m_EditorConfig.getGlobal("viewport")["imageTint"]["g"],
		(float)m_EditorConfig.getGlobal("viewport")["imageTint"]["b"],
		(float)m_EditorConfig.getGlobal("viewport")["imageTint"]["a"],
	};
	m_ViewportSettings.m_ImageBorderColor = {
		(float)m_EditorConfig.getGlobal("viewport")["borderColor"]["r"],
		(float)m_EditorConfig.getGlobal("viewport")["borderColor"]["g"],
		(float)m_EditorConfig.getGlobal("viewport")["borderColor"]["b"],
		(float)m_EditorConfig.getGlobal("viewport")["borderColor"]["a"],
	};

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigDockingWithShift = true;

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(RenderingDevice::GetSingleton()->getDevice(), RenderingDevice::GetSingleton()->getContext());
	ImGui::StyleColorsDark();
}

void Editor::start(VisualComponentGraph* visualGraph)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	applyDefaultUI();
	applyDocks();
}

void Editor::end(VisualComponentGraph* visualGraph)
{
	ImGui::PopStyleColor(m_EditorStyleColorPushCount);
	ImGui::PopStyleVar(m_EditorStyleVarPushCount);

	RenderingDevice::GetSingleton()->setTextureRenderTarget();
	RenderSystem::GetSingleton()->render(visualGraph);
	RenderingDevice::GetSingleton()->setBackBufferRenderTarget();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

Variant Editor::quit(const Event* event)
{
	for (auto&& arg : Extract(VariantVector, event->getData()))
	{
		OS::Print(Extract(String, arg));
	}
	PostQuitMessage(0);

	return String("Rootex Editor is quitting");
}

Editor::~Editor()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Editor::applyDefaultUI()
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

	applyMainMenu(windowFlags, optFullscreen, dockspaceFlags);
}

void Editor::applyMainMenu(const ImGuiWindowFlags& windowFlags, bool optFullscreen, const ImGuiDockNodeFlags& dockspaceFlags)
{
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
					Variant v = EventManager::GetSingleton()->returnCall("EditorQuitEvent", "EditorQuit", VariantVector({ String("SDSLabs"), String("IITR") }));
					OS::Print(Extract(String, v));
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::BeginMenu("Windows"))
				{
					ImGui::Checkbox("Toolbar", &m_ToolbarSettings.m_IsActive);
					ImGui::Checkbox("Output", &m_OutputSettings.m_IsActive);
					ImGui::Checkbox("Hierarchy", &m_HierarchySettings.m_IsActive);
					ImGui::Checkbox("Viewport", &m_ViewportSettings.m_IsActive);
					ImGui::Checkbox("File System", &m_FileSystemSettings.m_IsActive);
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

void Editor::applyDocks()
{
	if (m_FileSystemSettings.m_IsActive)
		applyFileSystemDock();
	if (m_ToolbarSettings.m_IsActive)
		applyToolbarDock();
	if (m_HierarchySettings.m_IsActive)
		applyHierarchyDock();
	if (m_InspectorSettings.m_IsActive)
		applyInspectorDock();
	if (m_OutputSettings.m_IsActive)
		applyOutputDock();
	if (m_ViewportSettings.m_IsActive)
		applyViewportDock();
}

void Editor::applyFileSystemDock()
{
	if (ImGui::Begin("File System"))
	{
	}
	ImGui::End();
}

void Editor::applyInspectorDock()
{
	if (ImGui::Begin("Inspector"))
	{
	}
	ImGui::End();
}

void Editor::applyOutputDock()
{
	if (ImGui::Begin("Output"))
	{
		for (auto&& log : m_Logs)
		{
			ImGui::Text(log.c_str());
		}
	}
	ImGui::End();
}

void Editor::applyViewportDock()
{
	ImGui::SetNextWindowBgAlpha(1.0f);
	if (ImGui::Begin("Viewport"))
	{
		ImVec2 region = ImGui::GetContentRegionAvail();
		if (region.x / region.y != m_ViewportSettings.m_AspectRatio)
		{
			region.y = region.x / m_ViewportSettings.m_AspectRatio;
		}

		m_ViewportSettings.m_ImageSize = region;

		ImGui::Image(
		    RenderingDevice::GetSingleton()->getRenderTextureShaderResourceView(),
		    m_ViewportSettings.m_ImageSize,
		    { 0, 0 },
		    { 1, 1 },
		    m_ViewportSettings.m_ImageTint,
		    m_ViewportSettings.m_ImageBorderColor);
	}
	ImGui::End();
}

void Editor::applyToolbarDock()
{
	if (ImGui::Begin("Toolbar"))
	{
	}
	ImGui::End();
}

void Editor::applyHierarchyDock()
{
	if (ImGui::Begin("Hierarchy"))
	{
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
	ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, m_Colors.m_MediumAccent);
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
	m_Logs.emplace_back(log);
}
