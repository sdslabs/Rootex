#include "hierarchy_dock.h"

#include "editor/editor.h"

#include "framework/systems/hierarchy_system.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

void HierarchyDock::showHierarchySubTree(Ref<Entity> node)
{
	Ref<HierarchyComponent> hierarchy = node->getComponent<HierarchyComponent>();
	if (hierarchy)
	{
		if (ImGui::TreeNodeEx(("##" + std::to_string(node->getID())).c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | (hierarchy->getChildren().size() ? ImGuiTreeNodeFlags_None : ImGuiTreeNodeFlags_Leaf)))
		{
			ImGui::SameLine();
			if (ImGui::Selectable((node->getName() + " #" + std::to_string(node->getID())).c_str(), m_OpenedEntityID == node->getID()))
			{
				m_OpenedEntityID = node->getID();
				PRINT("Viewed " + node->getName() + " through Hierarchy Dock");
				EventManager::GetSingleton()->call("OpenEntity", "EditorOpenEntity", node);
			}

			for (auto& child : node->getComponent<HierarchyComponent>()->getChildren())
			{
				showHierarchySubTree(child);
			}

			ImGui::TreePop();
		}
	}
}

Variant HierarchyDock::selectOpenEntity(const Event* event)
{
	m_OpenedEntityID = Extract(Ref<Entity>, event->getData())->getID();
	return true;
}

HierarchyDock::HierarchyDock()
{
	BIND_EVENT_MEMBER_FUNCTION("EditorOpenEntity", HierarchyDock::selectOpenEntity);
}

void HierarchyDock::draw()
{
	if (m_HierarchySettings.m_IsActive)
	{
		if (ImGui::Begin("Hierarchy"))
		{
			RootHierarchyComponent* rootComponent = HierarchySystem::GetSingleton()->getRootHierarchyComponent().get();
			showHierarchySubTree(rootComponent->getOwner());
		}
		ImGui::End();
	}
}
