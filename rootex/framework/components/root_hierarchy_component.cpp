#include "root_hierarchy_component.h"

#include "entity_factory.h"
#include "resource_loader.h"
#include "visual/sky_box_helper.h"

RootHierarchyComponent::RootHierarchyComponent()
{
	m_Children.reserve((size_t)RenderPass::End);
}

RootHierarchyComponent::~RootHierarchyComponent()
{
}

bool RootHierarchyComponent::isVisible(VisualComponentGraph* graph)
{
	return false;
}

bool RootHierarchyComponent::preRender(VisualComponentGraph* graph)
{
	return true;
}

void RootHierarchyComponent::renderPassRender(HierarchyComponent& renderPassGraph, VisualComponentGraph* graph)
{
	for (auto& child : renderPassGraph.getChildren())
	{
		VisualComponent* childVisualComponent = child->getComponent<VisualComponent>();

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

void RootHierarchyComponent::renderChildren(VisualComponentGraph* graph)
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

void RootHierarchyComponent::postRender(VisualComponentGraph* graph)
{
}

bool RootHierarchyComponent::addChild(Ref<Entity> child)
{
	RenderPass pass = child->getComponent<VisualComponent>()->getAttributes()->getRenderPass();

	HierarchyComponent::addChild(child);

	switch (pass)
	{
	case RenderPass::Global:
		m_GlobalGroup.addChild(child);
		return true;
		break;
	case RenderPass::Background:
		m_SkyGroup.addChild(child);
		return true;
		break;
	case RenderPass::Static:
		m_StaticGroup.addChild(child);
		return true;
		break;
	case RenderPass::Dynamic:
		m_EntityGroup.addChild(child);
		return true;
		break;
	case RenderPass::Editor:
		m_EditorGroup.addChild(child);
		return true;
		break;
	default:
		break;
	}
	return false;
}
