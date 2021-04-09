#include "spot_light_component.h"

#include "entity.h"
#include "systems/render_system.h"

SpotLightComponent::SpotLightComponent(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_DependencyOnTransformComponent(this)
{
	m_SpotLight.ambientColor = data.value("diffuseColor", Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_SpotLight.attConst = data.value("attConst", 0.045f);
	m_SpotLight.attLin = data.value("attLin", 1.0f);
	m_SpotLight.attQuad = data.value("attQuad", 0.0075f);
	m_SpotLight.diffuseColor = data.value("diffuseColor", Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_SpotLight.diffuseIntensity = data.value("diffuseIntensity", 1.0f);
	m_SpotLight.range = data.value("range", 10.0f);
	m_SpotLight.spot = data.value("spot", 4.0f);
	m_SpotLight.angleRange = data.value("angleRange", DirectX::XMConvertToRadians(30.0f));
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
	RenderSystem::GetSingleton()->submitCone(getTransformComponent()->getAbsoluteTransform(), m_SpotLight.range, m_SpotLight.angleRange * m_SpotLight.range);

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
