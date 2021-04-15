#include "render_ui_component.h"

#include "systems/render_ui_system.h"
#include "render_ui_component.h"

RenderUIComponent::RenderUIComponent(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_IsVisible(data.value("isVisible", true))
    , m_DependencyOnTransformComponent(this)
{
}

bool RenderUIComponent::preRender()
{
	RenderUISystem::GetSingleton()->pushUIMatrix(getTransformComponent()->getLocalTransform());
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
