#include "ui_component.h"

#include "systems/render_ui_system.h"
#include "ui_component.h"

UIComponent::UIComponent(bool isVisible)
    : m_IsVisible(isVisible)
{
}

bool UIComponent::setup()
{
	bool status = true;

	m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
	if (!m_TransformComponent)
	{
		ERR("TransformComponent not found on UIComponent");
		return false;
	}

	return status;
}

bool UIComponent::preRender()
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

void UIComponent::postRender()
{
	RenderUISystem::GetSingleton()->popUIMatrix();
}

JSON::json UIComponent::getJSON() const
{
	JSON::json j;

	j["isVisible"] = m_IsVisible;

	return j;
}
