#include "hierarchy_dock.h"

#include "editor/editor_system.h"

#include "framework/systems/hierarchy_system.h"
#include "framework/components/transform_component.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

void HierarchyDock::showHierarchySubTree(HierarchyComponent* hierarchy)
{
	if (hierarchy)
	{
		Ref<Entity> node = hierarchy->getOwner();

		if (ImGui::TreeNodeEx(("##" + std::to_string(node->getID())).c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | (hierarchy->getChildren().size() ? ImGuiTreeNodeFlags_None : ImGuiTreeNodeFlags_Leaf)))
		{
			ImGui::SameLine();

			if (hierarchy->getOwner()->isEditorOnly())
			{
				ImGui::PushStyleColor(ImGuiCol_Text, EditorSystem::GetSingleton()->getColors().warning);
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, EditorSystem::GetSingleton()->getColors().text);
			}

			if (!node->isEditorOnly() || m_IsShowEditorEntities)
			{
				if (ImGui::Selectable(node->getFullName().c_str(), m_OpenedEntityID == node->getID()))
				{
					openEntity(node);
				}

				if (ImGui::BeginPopupContextItem())
				{
					InspectorDock::GetSingleton()->drawEntityActions(node);
					ImGui::EndPopup();
				}

				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("RearrangeEntity", &node, sizeof(Ref<Entity>));
					ImGui::Text(node->getFullName().c_str());
					ImGui::EndDragDropSource();
				}

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityClass"))
					{
						const char* newEntityFile = (const char*)payload->Data;
						TextResourceFile* entityClassFile = ResourceLoader::CreateTextResourceFile(newEntityFile);
						Ref<Entity> entity = EntityFactory::GetSingleton()->createEntityFromClass(entityClassFile);
						node->getComponent<HierarchyComponent>()->snatchChild(entity);
						openEntity(entity);
					}

					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RearrangeEntity"))
					{
						Ref<Entity> rearrangeEntity = *(Ref<Entity>*)(payload->Data);
						node->getComponent<HierarchyComponent>()->snatchChild(rearrangeEntity);
						rearrangeEntity->getComponent<TransformComponent>()->setTransform(
						    rearrangeEntity->getComponent<TransformComponent>()->getAbsoluteTransform()
						    * node->getComponent<TransformComponent>()->getAbsoluteTransform().Invert()
						);
						openEntity(rearrangeEntity);
					}
					ImGui::EndDragDropTarget();
				}
			}

			ImGui::PopStyleColor(1);

			for (auto& child : node->getComponent<HierarchyComponent>()->getChildren())
			{
				showHierarchySubTree(child);
			}

			ImGui::TreePop();
		}
	}
}

void HierarchyDock::openEntity(Ref<Entity> entity)
{
	m_OpenedEntityID = entity->getID();
	PRINT("Viewed " + entity->getFullName() + " through Hierarchy Dock");
	EventManager::GetSingleton()->call("OpenEntity", "EditorOpenEntity", entity);
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

void HierarchyDock::draw(float deltaMilliseconds)
{
	if (m_HierarchySettings.m_IsActive)
	{
		if (ImGui::Begin("Hierarchy"))
		{
			ImGui::Checkbox("Show Editor Entities", &m_IsShowEditorEntities);

			HierarchyComponent* rootComponent = HierarchySystem::GetSingleton()->getRootHierarchyComponent().get();
			showHierarchySubTree(rootComponent);
		}
		ImGui::End();
	}
	if (m_HierarchySettings.m_IsEntitiesDockActive)
	{
		if (ImGui::Begin("Entities"))
		{
			ImGui::Checkbox("List Editor Entities", &m_IsShowEditorEntities);

			ImGui::Columns(2);
			ImGui::Text("Entity");
			ImGui::NextColumn();
			ImGui::Text("Components");
			ImGui::NextColumn();

			showEntities(EntityFactory::GetSingleton()->getEntities());

			ImGui::Columns(1);
		}
		ImGui::End();
	}
}

void HierarchyDock::showEntities(const HashMap<EntityID, Ref<Entity>>& entities)
{
	for (auto&& [entityID, entity] : entities)
	{
		if (entity->isEditorOnly() == m_IsShowEditorEntities)
		{
			ImGui::Separator();
			if (ImGui::Selectable(entity->getFullName().c_str(), m_OpenedEntityID == entity->getID()))
			{
				openEntity(entity);
			}
			if (ImGui::BeginPopupContextItem())
			{
				InspectorDock::GetSingleton()->drawEntityActions(entity);
				ImGui::EndPopup();
			}
			ImGui::NextColumn();
			HashMap<ComponentID, Ref<Component>> components = entity->getAllComponents();
			HashMap<ComponentID, Ref<Component>>::iterator it = components.begin();
			while (it != components.end())
			{
				bool increment = true;
				Ref<Component> component = it->second;
				ImGui::Selectable(component->getName());
				if (ImGui::BeginPopupContextItem(("Delete" + entity->getFullName() + component->getName()).c_str()))
				{
					if (ImGui::Button("Delete Component"))
					{
						if (component)
						{
							String componentName = component->getName();
							entity->removeComponent(component);
							PRINT("Deleted " + componentName + " from " + entity->getName());
							entity->setupComponents();
							increment = false;
						}
						else
						{
							ERR("Component not found: Possible level files corruption");
						}
					}
					ImGui::EndPopup();
				}
				if (increment)
				{
					it++;
				}
			}
			ImGui::NextColumn();
		}
	}
}
