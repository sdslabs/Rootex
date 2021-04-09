#include "point_light_component.h"

#include "entity.h"
#include "systems/render_system.h"

PointLightComponent::PointLightComponent(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_DependencyOnTransformComponent(this)
{
	m_PointLight.ambientColor = data.value("diffuseColor", Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_PointLight.attConst = data.value("attConst", 0.045f);
	m_PointLight.attLin = data.value("attLin", 1.0f);
	m_PointLight.attQuad = data.value("attQuad", 0.0075f);
	m_PointLight.diffuseColor = data.value("diffuseColor", Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_PointLight.diffuseIntensity = data.value("diffuseIntensity", 1.0f);
	m_PointLight.range = data.value("range", 10.0f);
}

JSON::json PointLightComponent::getJSON() const
{
	JSON::json j;

	j["attConst"] = m_PointLight.attConst;
	j["attLin"] = m_PointLight.attLin;
	j["attQuad"] = m_PointLight.attQuad;
	j["range"] = m_PointLight.range;
	j["diffuseIntensity"] = m_PointLight.diffuseIntensity;
	j["diffuseColor"] = m_PointLight.diffuseColor;
	j["ambientColor"] = m_PointLight.ambientColor;

	return j;
}

void PointLightComponent::draw()
{
	RenderSystem::GetSingleton()->submitSphere(getTransformComponent()->getAbsoluteTransform().Translation(), m_PointLight.range);

	ImGui::DragFloat("Diffuse Intensity##Point", &m_PointLight.diffuseIntensity, 0.1f);
	ImGui::ColorEdit4("Diffuse Color##Point", &m_PointLight.diffuseColor.x);
	ImGui::ColorEdit4("Ambient Color##Point", &m_PointLight.ambientColor.x);
	ImGui::DragFloat("Constant Attenuation##Point", &m_PointLight.attConst, 0.01f);
	ImGui::DragFloat("Linear Attenuation##Point", &m_PointLight.attLin, 0.01f);
	ImGui::DragFloat("Quadratic Attenuation##Point", &m_PointLight.attQuad, 0.01f);
	ImGui::DragFloat("Range##Point", &m_PointLight.range, 0.1f);
}
