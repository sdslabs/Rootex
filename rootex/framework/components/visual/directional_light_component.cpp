#include "directional_light_component.h"

Component* DirectionalLightComponent::Create(const JSON::json& componentData)
{
	DirectionalLightComponent* directionalLightComponent = new DirectionalLightComponent(
	    componentData.value("diffuseIntensity", 0.8f),
	    componentData.value("diffuseColor", Color(1.0f, 1.0f, 0.5f, 1.0f)),
	    componentData.value("ambientColor", Color(1.0f, 1.0f, 0.5f, 1.0f)));
	return directionalLightComponent;
}

DirectionalLightComponent::DirectionalLightComponent(const float diffuseIntensity,
    const Color& diffuseColor, const Color& ambientColor)
    : m_DependencyOnTransformComponent(this)
{
	m_DirectionalLight.ambientColor = ambientColor;
	m_DirectionalLight.diffuseColor = diffuseColor;
	m_DirectionalLight.diffuseIntensity = diffuseIntensity;
}

JSON::json DirectionalLightComponent::getJSON() const
{
	JSON::json j;

	j["diffuseIntensity"] = m_DirectionalLight.diffuseIntensity;
	j["diffuseColor"] = m_DirectionalLight.diffuseColor;
	j["ambientColor"] = m_DirectionalLight.ambientColor;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void DirectionalLightComponent::draw()
{
	ImGui::DragFloat("Diffuse Intensity##Directional", &m_DirectionalLight.diffuseIntensity, 0.1f);
	ImGui::ColorEdit4("Diffuse Color##Directional", &m_DirectionalLight.diffuseColor.x);
	ImGui::ColorEdit4("Ambient Color##Directional", &m_DirectionalLight.ambientColor.x);
}
#endif // ROOTEX_EDITOR
