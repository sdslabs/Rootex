#include "directional_light_component.h"

DEFINE_COMPONENT(DirectionalLightComponent);

DirectionalLightComponent::DirectionalLightComponent(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_DirectionalLight(
          { data.value("diffuseIntensity", 0.8f),
              data.value("diffuseColor", Color(1.0f, 1.0f, 0.5f, 1.0f)),
              data.value("ambientColor", Color(1.0f, 1.0f, 0.5f, 1.0f)) })
    , m_DependencyOnTransformComponent(this)
{
}

JSON::json DirectionalLightComponent::getJSON() const
{
	JSON::json j;

	j["diffuseIntensity"] = m_DirectionalLight.diffuseIntensity;
	j["diffuseColor"] = m_DirectionalLight.diffuseColor;
	j["ambientColor"] = m_DirectionalLight.ambientColor;

	return j;
}

void DirectionalLightComponent::draw()
{
	ImGui::DragFloat("Diffuse Intensity##Directional", &m_DirectionalLight.diffuseIntensity, 0.1f);
	ImGui::ColorEdit4("Diffuse Color##Directional", &m_DirectionalLight.diffuseColor.x);
	ImGui::ColorEdit4("Ambient Color##Directional", &m_DirectionalLight.ambientColor.x);
}
