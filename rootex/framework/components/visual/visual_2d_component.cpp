#include "visual_2d_component.h"

#include "systems/render_system.h"

Visual2DComponent::Visual2DComponent()
    : VisualComponent(RenderPass::UI, nullptr, nullptr)
{
}

bool Visual2DComponent::preRender(HierarchyGraph* graph)
{
	if (m_Attributes.m_TransformComponent)
	{
		RenderSystem::GetSingleton()->pushUIMatrix(m_Attributes.getTransform());
	}
	else
	{
		RenderSystem::GetSingleton()->pushUIMatrix(Matrix::Identity);
	}

	return true;
}

void Visual2DComponent::renderChildren(HierarchyGraph* graph)
{
	for (auto& child : m_Owner->getComponent<HierarchyComponent>()->getChildren())
	{
		Visual2DComponent* childVisualComponent = child->getComponent<Visual2DComponent>().get();

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

void Visual2DComponent::postRender(HierarchyGraph* graph)
{
	RenderSystem::GetSingleton()->popUIMatrix();
}
