#include "point_light_component.h"

#include "entity.h"

Component* PointLightComponent::Create(const JSON::json& componentData)
{
	PointLightComponent* pointLightComponent = new PointLightComponent(
	    (float)componentData["attConst"],
	    (float)componentData["attLin"],
	    (float)componentData["attQuad"],
	    (float)componentData["range"],
	    (float)componentData["diffuseIntensity"],
	    Color((float)componentData["diffuseColor"]["r"], (float)componentData["diffuseColor"]["g"],
	        (float)componentData["diffuseColor"]["b"], (float)componentData["diffuseColor"]["a"]),
	    Color((float)componentData["ambientColor"]["r"], (float)componentData["ambientColor"]["g"],
	        (float)componentData["ambientColor"]["b"], (float)componentData["ambientColor"]["a"]));

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

	j["diffuseColor"]["r"] = m_PointLight.diffuseColor.x;
	j["diffuseColor"]["g"] = m_PointLight.diffuseColor.y;
	j["diffuseColor"]["b"] = m_PointLight.diffuseColor.z;
	j["diffuseColor"]["a"] = m_PointLight.diffuseColor.w;

	j["ambientColor"]["r"] = m_PointLight.ambientColor.x;
	j["ambientColor"]["g"] = m_PointLight.ambientColor.y;
	j["ambientColor"]["b"] = m_PointLight.ambientColor.z;
	j["ambientColor"]["a"] = m_PointLight.ambientColor.w;

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
