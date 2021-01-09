#include "render_ui_component.h"

#include "systems/render_ui_system.h"
#include "render_ui_component.h"

RenderUIComponent::RenderUIComponent(bool isVisible)
    : m_IsVisible(isVisible)
    , m_DependencyOnTransformComponent(this)
{
}

void RenderUIComponent::RegisterAPI(sol::table& rootex)
{
	sol::usertype<RenderUIComponent> renderUIComponent = rootex.new_usertype<RenderUIComponent>(
	    "RenderUIComponent",
	    sol::base_classes, sol::bases<Component>());
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
