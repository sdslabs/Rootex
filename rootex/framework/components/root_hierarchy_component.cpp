#include "root_hierarchy_component.h"

#include "entity_factory.h"
#include "resource_loader.h"
#include "visual/sky_box_helper.h"

Component* RootHierarchyComponent::Create(const JSON::json& componentData)
{
	HierarchyComponent* component = new RootHierarchyComponent(INVALID_ID, {});
	return component;
}

Component* RootHierarchyComponent::CreateDefault()
{
	RootHierarchyComponent* component = new RootHierarchyComponent(INVALID_ID, {});
	return component;
}

RootHierarchyComponent::RootHierarchyComponent(EntityID parentID, const Vector<EntityID>& childrenIDs)
    : HierarchyComponent(parentID, childrenIDs)
    , m_StaticGroup(new HierarchyComponent(parentID, childrenIDs))
    , m_EntityGroup(new HierarchyComponent(parentID, childrenIDs))
    , m_GlobalGroup(new HierarchyComponent(parentID, childrenIDs))
    , m_SkyGroup(new HierarchyComponent(parentID, childrenIDs))
    , m_EditorGroup(new HierarchyComponent(parentID, childrenIDs))
{
}

RootHierarchyComponent::~RootHierarchyComponent()
{
}

bool RootHierarchyComponent::isVisible(HierarchyGraph* graph)
{
	return false;
}

bool RootHierarchyComponent::preRender(HierarchyGraph* graph)
{
	return true;
}

void RootHierarchyComponent::renderPassRender(Ref<HierarchyComponent> renderPassGraph, HierarchyGraph* graph)
{
	for (auto& child : renderPassGraph->getChildren())
	{
		Ref<VisualComponent> childVisualComponent = child->getComponent<VisualComponent>();

		if (childVisualComponent)
		{
			childVisualComponent->preRender(graph);

			if (childVisualComponent->isVisible(graph))
			{
				// Assumed to be opaque
				childVisualComponent->render(graph);
			}
			childVisualComponent->renderChildren(graph);

			childVisualComponent->postRender(graph);
		}
	}
}

void RootHierarchyComponent::renderChildren(HierarchyGraph* graph)
{
	renderPassRender(m_GlobalGroup, graph);
	renderPassRender(m_StaticGroup, graph);
	renderPassRender(m_EntityGroup, graph);
	renderPassRender(m_EditorGroup, graph);
	{
		SkyBoxHelper skyBoxHelper;
		renderPassRender(m_SkyGroup, graph);
	}
}

void RootHierarchyComponent::postRender(HierarchyGraph* graph)
{
}

bool RootHierarchyComponent::addChild(Ref<Entity> child)
{
	HierarchyComponent::addChild(child);

	Ref<VisualComponent> vc = child->getComponent<VisualComponent>();
	if (vc)
	{
		RenderPass pass = vc->getAttributes()->getRenderPass();
		switch (pass)
		{
		case RenderPass::Global:
			m_GlobalGroup->addChild(child);
			return true;
			break;
		case RenderPass::Background:
			m_SkyGroup->addChild(child);
			return true;
			break;
		case RenderPass::Static:
			m_StaticGroup->addChild(child);
			return true;
			break;
		case RenderPass::Dynamic:
			m_EntityGroup->addChild(child);
			return true;
			break;
		case RenderPass::Editor:
			m_EditorGroup->addChild(child);
			return true;
			break;
		default:
			break;
		}
		return false;
	}
	return true;
}

void RootHierarchyComponent::clear()
{
	m_ChildrenIDs.clear();
	m_Children.clear();

	m_StaticGroup->m_ChildrenIDs.clear();
	m_StaticGroup->m_Children.clear();
	
	m_GlobalGroup->m_ChildrenIDs.clear();
	m_GlobalGroup->m_Children.clear();
	
	m_EntityGroup->m_ChildrenIDs.clear();
	m_EntityGroup->m_Children.clear();
	
	m_SkyGroup->m_ChildrenIDs.clear();
	m_SkyGroup->m_Children.clear();
	
	m_EditorGroup->m_ChildrenIDs.clear();
	m_EditorGroup->m_Children.clear();
}

JSON::json RootHierarchyComponent::getJSON() const
{
	return JSON::json::object();
}
