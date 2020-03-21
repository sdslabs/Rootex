#include "directional_light_component.h"

Component* DirectionalLightComponent::Create(const LuaVariable& componentData)
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
    : m_direction(direction)
    , m_ambientColor(ambientColor)
    , m_diffuseColor(diffuseColor)
    , m_diffuseIntensity(diffuseIntensity)
{
}

DirectionalLightComponent::~DirectionalLightComponent()
{
}