#include "point_light_component.h"

#include "entity.h"

Component* PointLightComponent::Create(const JSON::json& componentData)
{
	PointLightComponent* pointLightComponent = new PointLightComponent(
	    componentData.value("attConst", 0.045f),
	    componentData.value("attLin", 1.0f),
	    componentData.value("attQuad", 0.0075f),
	    componentData.value("range", 10.0f),
	    componentData.value("diffuseIntensity", 1.0f),
	    componentData.value("diffuseColor", Color(1.0f, 1.0f, 1.0f, 1.0f)),
	    componentData.value("ambientColor", Color(0.5f, 0.5f, 0.5f, 1.0f)));
	return pointLightComponent;
}

Component* PointLightComponent::CreateDefault()
{
	PointLightComponent* pointLightComponent = new PointLightComponent(
	    0.045f,
	    1.0f,
	    0.0075f,
	    10.0f,
	    1.0f,
	    Color(1.0f, 1.0f, 1.0f, 1.0f),
	    Color(0.5f, 0.5f, 0.5f, 1.0f));
	return pointLightComponent;
}

PointLightComponent::PointLightComponent(const float constAtt, const float linAtt, const float quadAtt,
    const float range, const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor)
    : m_DependencyOnTransformComponent(this)
{
	m_PointLight.ambientColor = ambientColor;
	m_PointLight.attConst = constAtt;
	m_PointLight.attLin = linAtt;
	m_PointLight.attQuad = quadAtt;
	m_PointLight.diffuseColor = diffuseColor;
	m_PointLight.diffuseIntensity = diffuseIntensity;
	m_PointLight.range = range;
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

#ifdef ROOTEX_EDITOR
#include"imgui.h"
#include "systems/render_system.h"
void PointLightComponent::draw()
{
	RenderSystem::GetSingleton()->submitSphere(m_TransformComponent->getAbsoluteTransform().Translation(), m_PointLight.range);

	ImGui::DragFloat("Diffuse Intensity##Point", &m_PointLight.diffuseIntensity, 0.1f);
	ImGui::ColorEdit4("Diffuse Color##Point", &m_PointLight.diffuseColor.x);
	ImGui::ColorEdit4("Ambient Color##Point", &m_PointLight.ambientColor.x);
	ImGui::DragFloat("Constant Attenuation##Point", &m_PointLight.attConst, 0.01f);
	ImGui::DragFloat("Linear Attenuation##Point", &m_PointLight.attLin, 0.01f);
	ImGui::DragFloat("Quadratic Attenuation##Point", &m_PointLight.attQuad, 0.01f);
	ImGui::DragFloat("Range##Point", &m_PointLight.range, 0.1f);
}
#endif // ROOTEX_EDITOR
