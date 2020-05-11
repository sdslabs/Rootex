#include "visual_2d_component.h"

#include "systems/render_system.h"

UIComponent::UIComponent()
	:VisualComponent(RenderPassUI, true)
{
}

bool UIComponent::preRender()
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

void UIComponent::postRender()
{
	RenderSystem::GetSingleton()->popUIMatrix();
}
