#include "render_ui_component.h"

#include "systems/render_ui_system.h"
#include "render_ui_component.h"

RenderUIComponent::RenderUIComponent(bool isVisible)
    : m_IsVisible(isVisible)
{
}

void RenderUIComponent::RegisterAPI(sol::table& rootex)
{
	sol::usertype<RenderUIComponent> renderUIComponent = rootex.new_usertype<RenderUIComponent>(
	    "RenderUIComponent",
	    sol::base_classes, sol::bases<Component>());
	rootex["Entity"]["getRenderUI"] = &Entity::getComponent<RenderUIComponent>;
}

bool RenderUIComponent::setup()
{
	m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
	if (!m_TransformComponent)
	{
		ERR("TransformComponent not found on RenderUIComponent");
		return false;
	}
	return true;
}

bool RenderUIComponent::preRender()
{
	if (m_TransformComponent)
	{
		RenderUISystem::GetSingleton()->pushUIMatrix(m_TransformComponent->getLocalTransform());
	}
	else
	{
		RenderUISystem::GetSingleton()->pushUIMatrix(Matrix::Identity);
	}

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
