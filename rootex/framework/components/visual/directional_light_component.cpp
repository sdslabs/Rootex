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
	    Vector3(0.5f, 0.5f, 0.5f),
	    0.8f,
	    Color(0.3f, 0.1f, 0.4f, 1.0f),
	    Color(0.05f, 0.05f, 0.05f, 1.0f));

	return directionalLightComponent;
}

DirectionalLightComponent::DirectionalLightComponent(const Vector3& direction, const float diffuseIntensity, 
	const Color& diffuseColor, const Color& ambientColor)
    : m_Direction(direction)
    , m_AmbientColor(ambientColor)
    , m_DiffuseColor(diffuseColor)
    , m_DiffuseIntensity(diffuseIntensity)
{
}

DirectionalLightComponent::~DirectionalLightComponent()
{
}

JSON::json DirectionalLightComponent::getJSON() const
{
	JSON::json j;

	j["direction"]["x"] = m_Direction.x;
	j["direction"]["y"] = m_Direction.y;
	j["direction"]["z"] = m_Direction.z;

	j["diffuseIntensity"] = m_DiffuseIntensity;

	j["diffuseColor"]["r"] = m_DiffuseColor.x;
	j["diffuseColor"]["g"] = m_DiffuseColor.y;
	j["diffuseColor"]["b"] = m_DiffuseColor.z;
	j["diffuseColor"]["a"] = m_DiffuseColor.w;

	j["ambientColor"]["r"] = m_AmbientColor.x;
	j["ambientColor"]["g"] = m_AmbientColor.y;
	j["ambientColor"]["b"] = m_AmbientColor.z;
	j["ambientColor"]["a"] = m_AmbientColor.w;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void DirectionalLightComponent::draw()
{
	ImGui::SliderFloat("Diffuse Intensity##Directional", &m_DiffuseIntensity, 0.0f, 1.0f);
	ImGui::ColorEdit4("Diffuse Color##Directional", &m_DiffuseColor.x);
	ImGui::ColorEdit4("Ambient Color##Directional", &m_AmbientColor.x);
	ImGui::DragFloat3("Direction##Directional", &m_Direction.x);
}
#endif // ROOTEX_EDITOR
