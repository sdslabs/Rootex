#include "spring_arm_component.h"

DEFINE_COMPONENT(SpringArmComponent);

SpringArmComponent::SpringArmComponent(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_DesiredLocalPosition(data.value("desiredLocalPosition", Vector3::Zero))
    , m_Lerp(data.value("lerp", 0.0f))
    , m_DependencyOnTransformComponent(this)
{
}

JSON::json SpringArmComponent::getJSON() const
{
	JSON::json j;

	j["lerp"] = m_Lerp;
	j["desiredLocalPosition"] = m_DesiredLocalPosition;

	return j;
}

void SpringArmComponent::draw()
{
	TransformComponent* transform = getTransformComponent();

	if (ImGui::DragFloat3("##DesiredLocalPosition", &m_DesiredLocalPosition.x, 0.01f))
	{

		transform->setPosition(m_DesiredLocalPosition);
	}
	ImGui::SameLine();
	if (ImGui::Button("DesiredLocalPosition"))
	{
		setDesiredLocalPosition({ 0.0f, 5.0f, 10.0f });
	}

	ImGui::SliderFloat("Lerp Factor", &m_Lerp, 0.0f, 1.0f);
}
