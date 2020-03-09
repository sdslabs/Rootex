#include "hierarchy_dock.h"

#include "framework/systems/render_system.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

void HierarchyDock::draw()
{
	if (m_HierarchySettings.m_IsActive)
	{
		if (ImGui::Begin("Hierarchy"))
		{
			//const HierarchyGraph& graph =  RenderSystem::GetSingleton()->getHierarchyGraph();
			//const RootHierarchyComponent* child = graph.getRoot();
			//child->
		}
		ImGui::End();
	}
}
