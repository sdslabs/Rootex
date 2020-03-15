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
    : m_constAtt(constAtt)
    , m_linAtt(linAtt)
    , m_quadAtt(quadAtt)
    , m_range(range)
    , m_ambientColor(ambientColor)
    , m_diffuseColor(diffuseColor)
    , m_diffuseIntensity(diffuseIntensity)
{
}

PointLightComponent::~PointLightComponent()
{
}

JSON::json PointLightComponent::getJSON() const
{
	JSON::json j;

	j["attConst"] = m_constAtt;
	j["attLin"] = m_linAtt;
	j["attQuad"] = m_quadAtt;
	j["range"] = m_range;
	j["diffuseIntensity"] = m_diffuseIntensity;

	j["diffuseColor"]["r"] = m_diffuseColor.x;
	j["diffuseColor"]["g"] = m_diffuseColor.y;
	j["diffuseColor"]["b"] = m_diffuseColor.z;
	j["diffuseColor"]["a"] = m_diffuseColor.w;

	j["ambientColor"]["r"] = m_ambientColor.x;
	j["ambientColor"]["g"] = m_ambientColor.y;
	j["ambientColor"]["b"] = m_ambientColor.z;
	j["ambientColor"]["a"] = m_ambientColor.w;

	return j;
}

#ifdef ROOTEX_EDITOR
#include"imgui.h"
void PointLightComponent::draw()
{
	ImGui::DragFloat("Diffuse Intensity##Point", &m_diffuseIntensity);
	ImGui::ColorEdit4("Diffuse Color##Point", &m_diffuseColor.x);
	ImGui::ColorEdit4("Ambient Color##Point", &m_ambientColor.x);
	ImGui::DragFloat("Constant Attenuation##Point", &m_constAtt);
	ImGui::DragFloat("Linear Attenuation##Point", &m_linAtt);
	ImGui::DragFloat("Quadratic Attenuation##Point", &m_quadAtt);
	ImGui::DragFloat("Range##Point", &m_range);
}
#endif // ROOTEX_EDITOR
