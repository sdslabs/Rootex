#include "spot_light_component.h"

#include "entity.h"
#include "systems/render_system.h"

Component* SpotLightComponent::Create(const JSON::json& componentData)
{
	SpotLightComponent* spotLightComponent = new SpotLightComponent(
	    componentData.value("attConst", 1.0f),
	    componentData.value("attLin", 0.045f),
	    componentData.value("attQuad", 0.075f),
	    componentData.value("range", 100.0f),
	    componentData.value("diffuseIntensity", 1.0f),
	    componentData.value("diffuseColor", Color(1.0f, 1.0f, 1.0f, 1.0f)),
	    componentData.value("ambientColor", Color(1.0f, 1.0f, 1.0f, 1.0f)),
	    componentData.value("spot", 4.0f),
		componentData.value("angleRange", DirectX::XMConvertToRadians(30.0f)));

	return spotLightComponent;
}

SpotLightComponent::SpotLightComponent(const float constAtt, const float linAtt, const float quadAtt,
    const float range, const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor,
	float spot, float angleRange)
    : m_DependencyOnTransformComponent(this)
{
	m_SpotLight.ambientColor = ambientColor;
	m_SpotLight.angleRange = angleRange;
	m_SpotLight.attConst = constAtt;
	m_SpotLight.attLin = linAtt;
	m_SpotLight.attQuad = quadAtt;
	m_SpotLight.diffuseColor = diffuseColor;
	m_SpotLight.diffuseIntensity = diffuseIntensity;
	m_SpotLight.spot = spot;
	m_SpotLight.range = range;
}

JSON::json SpotLightComponent::getJSON() const
{
	JSON::json j;

	j["attConst"] = m_SpotLight.attConst;
	j["attLin"] = m_SpotLight.attLin;
	j["attQuad"] = m_SpotLight.attQuad;
	j["range"] = m_SpotLight.range;
	j["diffuseIntensity"] = m_SpotLight.diffuseIntensity;
	j["diffuseColor"] = m_SpotLight.diffuseColor;
	j["ambientColor"] = m_SpotLight.ambientColor;
	j["spot"] = m_SpotLight.spot;
	j["angleRange"] = m_SpotLight.range;

	return j;
}

void SpotLightComponent::draw()
{
	RenderSystem::GetSingleton()->submitCone(m_TransformComponent->getAbsoluteTransform(), m_SpotLight.range, m_SpotLight.angleRange * m_SpotLight.range);

	ImGui::DragFloat("Diffuse Intensity##Spot", &m_SpotLight.diffuseIntensity, 0.1f);
	ImGui::ColorEdit4("Diffuse Color##Spot", &m_SpotLight.diffuseColor.x);
	ImGui::ColorEdit4("Ambient Color##Spot", &m_SpotLight.ambientColor.x);
	ImGui::DragFloat("Constant Attenuation##Spot", &m_SpotLight.attConst, 0.01f);
	ImGui::DragFloat("Linear Attenuation##Spot", &m_SpotLight.attLin, 0.01f);
	ImGui::DragFloat("Quadratic Attenuation##Spot", &m_SpotLight.attQuad, 0.01f);
	ImGui::DragFloat("Range##Spot", &m_SpotLight.range, 1.0f);
	ImGui::DragFloat("Spot Factor##Spot", &m_SpotLight.spot, 0.1f);
	ImGui::SliderAngle("Cone Sub-Angle##Spot", &m_SpotLight.angleRange, 0.0f, 90.0f);
}
