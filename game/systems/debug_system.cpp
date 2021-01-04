#include "debug_system.h"

#include "app/application.h"
#include "core/renderer/rendering_device.h"
#include "framework/scene_loader.h"
#include "framework/component.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

DebugSystem::DebugSystem()
    : System("DebugSystem", System::UpdateOrder::PostRender, true)
{
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

void DrawSceneTree(Scene* scene)
{
	if (ImGui::TreeNodeEx(scene->getFullName().c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_CollapsingHeader))
	{
		if (Entity* entity = scene->getEntity())
		{
			for (auto& [componentID, component] : entity->getAllComponents())
			{
				ImGui::Text(component->getName());
			}
		}

		ImGui::Indent();
		for (auto& child : scene->getChildren())
		{
			DrawSceneTree(child.get());
		}
		ImGui::Unindent();
	}
}

void DebugSystem::update(float deltaMilliseconds)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug Scene");
	DrawSceneTree(SceneLoader::GetSingleton()->getRootScene());
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
