#include "render_ui_component.h"

#include "systems/render_ui_system.h"
#include "render_ui_component.h"

RenderUIComponent::RenderUIComponent(bool isVisible)
    : m_IsVisible(isVisible)
    , m_DependencyOnTransformComponent(this)
{
}

bool RenderUIComponent::preRender()
{
	RenderUISystem::GetSingleton()->pushUIMatrix(m_TransformComponent->getLocalTransform());
	return true;
}

void RenderUIComponent::postRender()
{
	RenderUISystem::GetSingleton()->popUIMatrix();
}

JSON::json RenderUIComponent::getJSON() const
{
	JSON::json j;

	j["isVisible"] = m_IsVisible;

	return j;
}
