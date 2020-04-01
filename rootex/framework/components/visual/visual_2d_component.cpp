#include "visual_2d_component.h"

#include "systems/render_system.h"

Visual2DComponent::Visual2DComponent()
	:VisualComponent(RenderPassUI, true)
{
}

bool Visual2DComponent::preRender()
{
	if (m_TransformComponent)
	{
		RenderSystem::GetSingleton()->pushUIMatrix(getTransform());
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
