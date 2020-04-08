#include "point_light_component.h"

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
    : m_AttConst(constAtt)
    , m_AttLin(linAtt)
    , m_AttQuad(quadAtt)
    , m_Range(range)
    , m_AmbientColor(ambientColor)
    , m_DiffuseColor(diffuseColor)
    , m_DiffuseIntensity(diffuseIntensity)
{
}

PointLightComponent::~PointLightComponent()
{
}

JSON::json PointLightComponent::getJSON() const
{
	JSON::json j;

	j["attConst"] = m_AttConst;
	j["attLin"] = m_AttLin;
	j["attQuad"] = m_AttQuad;
	j["range"] = m_Range;
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
#include"imgui.h"
void PointLightComponent::draw()
{
	ImGui::DragFloat("Diffuse Intensity##Point", &m_DiffuseIntensity);
	ImGui::ColorEdit4("Diffuse Color##Point", &m_DiffuseColor.x);
	ImGui::ColorEdit4("Ambient Color##Point", &m_AmbientColor.x);
	ImGui::DragFloat("Constant Attenuation##Point", &m_AttConst);
	ImGui::DragFloat("Linear Attenuation##Point", &m_AttLin);
	ImGui::DragFloat("Quadratic Attenuation##Point", &m_AttQuad);
	ImGui::DragFloat("Range##Point", &m_Range);
}
#endif // ROOTEX_EDITOR
