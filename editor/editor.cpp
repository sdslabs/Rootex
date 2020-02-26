#include "editor.h"

#include "core/renderer/rendering_device.h"

#include "resource_loader.h"
#include "framework/components/visual/visual_component_graph.h"
#include "framework/components/hierarchy_component.h"
#include "framework/systems/render_system.h"

void Editor::initialize(HWND hWnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigDockingWithShift = true;

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(RenderingDevice::GetSingleton()->getDevice(), RenderingDevice::GetSingleton()->getContext());
	ImGui::StyleColorsDark();
}

void Editor::begin(VisualComponentGraph* visualGraph)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	applyDockspace();
	applyDocks();
}

void Editor::end(VisualComponentGraph* visualGraph)
{
	RenderingDevice::GetSingleton()->setTextureRenderTarget();
	RenderSystem::GetSingleton()->render(visualGraph);
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

void Editor::applyDockspace()
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
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		windowFlags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("Rootex Edtior", nullptr, windowFlags);
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
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Lua File", ""))
				{
					OS::PrintLine("Yay");
				}
				ImGui::MenuItem("Save");
				ImGui::MenuItem("Save All", "");
				ImGui::Separator();
				ImGui::MenuItem("Quit", "");
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (ImGui::BeginMenuBar())
		{
			static String menuAction = "";
			if (ImGui::BeginMenu("About"))
			{
				if (ImGui::MenuItem("Rootex Editor"))
				{
					menuAction = "Rootex Editor";
				}
				ImGui::EndMenu();
			}

			if (menuAction == "Rootex Editor")
			{
				ImGui::OpenPopup("About Rootex Editor");
			}

			if (ImGui::BeginPopup("About Rootex Editor", ImGuiWindowFlags_AlwaysAutoResize))
			{
				static TextResourceFile* license = ResourceLoader::CreateLuaTextResourceFile("LICENSE");
				ImGui::Text(license->getString().c_str());
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
	applyToolbarDock();
	applyViewportDock();
}

void Editor::applyViewportDock()
{
	if (ImGui::Begin("Viewport"))
	{
		m_ViewportWidth = ImGui::GetWindowWidth();
		m_ViewportHeight = ImGui::GetWindowHeight();

		ImGui::Image(RenderingDevice::GetSingleton()->getRenderTextureShaderResourceView(), { m_ViewportZoom * m_ViewportWidth, m_ViewportZoom * m_ViewportHeight });

		ImGui::Checkbox("Enable Zoom", &m_ViewportZoomEnabled);

		if (m_ViewportZoomEnabled)
		{
			ImGui::SliderFloat("Zoom", &m_ViewportZoom, 0.0f, 1.0f);
		}
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

Editor* Editor::GetSingleton()
{
	static Editor singleton;
	return &singleton;
}
