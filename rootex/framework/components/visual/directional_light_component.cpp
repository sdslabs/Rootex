#include "directional_light_component.h"

Component* DirectionalLightComponent::Create(const JSON::json& componentData)
{
	DirectionalLightComponent* directionalLightComponent = new DirectionalLightComponent(
	    (float)componentData["diffuseIntensity"],
	    Color((float)componentData["diffuseColor"]["r"], (float)componentData["diffuseColor"]["g"],
	        (float)componentData["diffuseColor"]["b"], (float)componentData["diffuseColor"]["a"]),
	    Color((float)componentData["ambientColor"]["r"], (float)componentData["ambientColor"]["g"],
	        (float)componentData["ambientColor"]["b"], (float)componentData["ambientColor"]["a"]));

	return directionalLightComponent;
}

Component* DirectionalLightComponent::CreateDefault()
{
	DirectionalLightComponent* directionalLightComponent = new DirectionalLightComponent(
	    0.8f,
	    Color(1.0f, 1.0f, 0.5f, 1.0f),
	    Color(0.8f, 0.8f, 0.5f, 1.0f));

	return directionalLightComponent;
}

DirectionalLightComponent::DirectionalLightComponent(const float diffuseIntensity,
    const Color& diffuseColor, const Color& ambientColor)
{
	m_DirectionalLight.ambientColor = ambientColor;
	m_DirectionalLight.diffuseColor = diffuseColor;
	m_DirectionalLight.diffuseIntensity = diffuseIntensity;
}

JSON::json DirectionalLightComponent::getJSON() const
{
	JSON::json j;

	j["diffuseIntensity"] = m_DirectionalLight.diffuseIntensity;

	j["diffuseColor"]["r"] = m_DirectionalLight.diffuseColor.x;
	j["diffuseColor"]["g"] = m_DirectionalLight.diffuseColor.y;
	j["diffuseColor"]["b"] = m_DirectionalLight.diffuseColor.z;
	j["diffuseColor"]["a"] = m_DirectionalLight.diffuseColor.w;

	j["ambientColor"]["r"] = m_DirectionalLight.ambientColor.x;
	j["ambientColor"]["g"] = m_DirectionalLight.ambientColor.y;
	j["ambientColor"]["b"] = m_DirectionalLight.ambientColor.z;
	j["ambientColor"]["a"] = m_DirectionalLight.ambientColor.w;

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
