#include "hierarchy_dock.h"

#include "editor/editor.h"

#include "framework/systems/hierarchy_system.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

void HierarchyDock::showHierarchySubTree(Ref<Entity> node)
{
	if (ImGui::TreeNodeEx(("##" + node->getName()).c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | (node->getComponent<HierarchyComponent>()->getChildren().size() ? ImGuiTreeNodeFlags_None : ImGuiTreeNodeFlags_Leaf)))
	{
		ImGui::SameLine();
		if (ImGui::Selectable(node->getName().c_str(), m_OpenedEntityName == node->getName()))
		{
			m_OpenedEntityName = node->getName();
			Editor::GetSingleton()->log("Viewed " + node->getName() + " through Hierarchy Dock");
			EventManager::GetSingleton()->call("OpenEntity", "EditorInspectorOpenEntity", node);
		}
		
		for (auto& child : node->getComponent<HierarchyComponent>()->getChildren())
		{
			showHierarchySubTree(child);
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
			RootHierarchyComponent* rootComponent = HierarchySystem::GetSingleton()->getRoot().get();
			showHierarchySubTree(rootComponent->getOwner());
		}
		ImGui::End();
	}
}
