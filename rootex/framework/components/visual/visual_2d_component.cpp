#include "visual_2d_component.h"

#include "systems/render_system.h"

Visual2DComponent::Visual2DComponent(bool visibility)
    : VisualComponent(RenderPassUI, nullptr, nullptr, visibility)
{
}

bool Visual2DComponent::preRender()
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

void Visual2DComponent::postRender()
{
	RenderSystem::GetSingleton()->popUIMatrix();
}
