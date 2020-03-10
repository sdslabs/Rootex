#include "hierarchy_dock.h"

#include "editor/editor.h"

#include "framework/systems/hierarchy_system.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

void HierarchyDock::showHierarchySubTree(HierarchyComponent* node)
{
	if (ImGui::TreeNodeEx(node->getOwner()->getName().c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | (node->getChildren().size() ? ImGuiTreeNodeFlags_None : ImGuiTreeNodeFlags_Leaf)))
	{
		if (ImGui::IsItemClicked())
		{
			EventManager::GetSingleton()->call("OpenEntity", "EditorInspectorOpenEntity", node->getOwner());
		}
		
		for (auto& child : node->getChildren())
		{
			showHierarchySubTree(child->getComponent<HierarchyComponent>());
		}
		ImGui::TreePop();
	}
}

void HierarchyDock::draw()
{
	if (m_HierarchySettings.m_IsActive)
	{
		if (ImGui::Begin("Hierarchy"))
		{
			RootHierarchyComponent* rootComponent = HierarchySystem::GetSingleton()->getRoot();
			showHierarchySubTree(rootComponent);
		}
		ImGui::End();
	}
}
