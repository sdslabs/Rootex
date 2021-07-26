#include "spring_arm_component.h"

DEFINE_COMPONENT(SpringArmComponent);

SpringArmComponent::SpringArmComponent(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_DependencyOnTransformComponent(this)
{
}

JSON::json SpringArmComponent::getJSON() const
{
	return JSON::json::object();
}

bool SpringArmComponent::setupData()
{
	m_DesiredLocalPosition = m_DependencyOnTransformComponent.getComponent()->getPosition();
	return true;
}

void SpringArmComponent::draw()
{
	TransformComponent* transform = this->getOwner().getComponent<TransformComponent>();

	if (ImGui::DragFloat3("##Position", &m_DesiredLocalPosition.x, 0.01f))
	{

		transform->setPosition(m_DesiredLocalPosition);
	}
	ImGui::SameLine();
	if (ImGui::Button("Position"))
	{
		setDesiredLocalPosition({ 0.0f, 5.0f, 10.0f });
	}

	ImGui::SliderFloat("Lerp Factor", &m_Lerp, 0.0f, 1.0f);
}
