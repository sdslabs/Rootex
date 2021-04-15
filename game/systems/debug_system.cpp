#include "debug_system.h"

#include "app/application.h"
#include "core/renderer/rendering_device.h"
#include "framework/scene_loader.h"
#include "framework/component.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

DebugSystem::DebugSystem()
    : System("DebugSystem", System::UpdateOrder::GameRender, true)
{
	m_FPSRecords.resize(100);
}

DebugSystem* DebugSystem::GetSingleton()
{
	static DebugSystem singleton;
	return &singleton;
}

bool DebugSystem::initialize(const JSON::json& systemData)
{
	setActive(systemData["active"]);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_DpiEnableScaleFonts | ImGuiConfigFlags_DpiEnableScaleViewports;
	io.ConfigDockingWithShift = true;
	io.FontAllowUserScaling = true;
	io.Fonts->AddFontDefault();

	ImGui_ImplWin32_Init(Application::GetSingleton()->getWindow()->getWindowHandle());
	ImGui_ImplDX11_Init(RenderingDevice::GetSingleton()->getDevice(), RenderingDevice::GetSingleton()->getContext());
	ImGui::StyleColorsDark();

	return true;
}

void DebugSystem::drawSceneTree(Scene* scene)
{
	ImGui::PushID(scene->getID());
	if (ImGui::TreeNodeEx(scene->getFullName().c_str(), ImGuiTreeNodeFlags_CollapsingHeader))
	{
		if (m_IsEditing)
		{
			Entity& entity = scene->getEntity();
			entity.draw();
			for (auto& [componentID, component] : entity.getAllComponents())
			{
				if (ImGui::TreeNodeEx(component->getName()))
				{
					component->draw();
					ImGui::TreePop();
				}
			}
		}

		ImGui::Indent();
		for (auto& child : scene->getChildren())
		{
			drawSceneTree(child.get());
		}
		ImGui::Unindent();
	}
	ImGui::PopID();
}

void DebugSystem::update(float deltaMilliseconds)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug Scene");

	float fps = 1.0f / (deltaMilliseconds * MS_TO_S);
	RootexFPSGraph("FPS", m_FPSRecords, fps);

	ImGui::Checkbox("Enable Editing", &m_IsEditing);
	drawSceneTree(SceneLoader::GetSingleton()->getRootScene());

	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
