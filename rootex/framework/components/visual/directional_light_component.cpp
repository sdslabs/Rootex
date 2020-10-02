#include "directional_light_component.h"

Component* DirectionalLightComponent::Create(const JSON::json& componentData)
{
	DirectionalLightComponent* directionalLightComponent = new DirectionalLightComponent(
	    Vector3((float)componentData["direction"]["x"],
	    (float)componentData["direction"]["y"], (float)componentData["direction"]["z"]),
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
	    Vector3(0.5f, -0.5f, 0.5f),
	    0.8f,
	    Color(1.0f, 1.0f, 0.5f, 1.0f),
	    Color(0.8f, 0.8f, 0.5f, 1.0f));

	return directionalLightComponent;
}

DirectionalLightComponent::DirectionalLightComponent(const Vector3& direction, const float diffuseIntensity,
    const Color& diffuseColor, const Color& ambientColor)
{
	m_DirectionalLight.ambientColor = ambientColor;
	m_DirectionalLight.diffuseColor = diffuseColor;
	m_DirectionalLight.diffuseIntensity = diffuseIntensity;
	m_DirectionalLight.direction = direction;
}

JSON::json DirectionalLightComponent::getJSON() const
{
	JSON::json j;

	j["direction"]["x"] = m_DirectionalLight.direction.x;
	j["direction"]["y"] = m_DirectionalLight.direction.y;
	j["direction"]["z"] = m_DirectionalLight.direction.z;

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
	ImGui::DragFloat3("Direction##Directional", &m_DirectionalLight.direction.x, 0.01f);
}
#endif // ROOTEX_EDITOR
