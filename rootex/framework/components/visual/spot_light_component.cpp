#include "spot_light_component.h"

Component* SpotLightComponent::Create(const JSON::json& componentData)
{
	SpotLightComponent* spotLightComponent = new SpotLightComponent(
	    (float)componentData["attConst"],
	    (float)componentData["attLin"],
	    (float)componentData["attQuad"],
	    (float)componentData["range"],
	    (float)componentData["diffuseIntensity"],
	    Color((float)componentData["diffuseColor"]["r"], (float)componentData["diffuseColor"]["g"],
	        (float)componentData["diffuseColor"]["b"], (float)componentData["diffuseColor"]["a"]),
	    Color((float)componentData["ambientColor"]["r"], (float)componentData["ambientColor"]["g"],
	        (float)componentData["ambientColor"]["b"], (float)componentData["ambientColor"]["a"]),
		(float)componentData["spot"], (float)componentData["angleRange"]);

	return spotLightComponent;
}

Component* SpotLightComponent::CreateDefault()
{
	SpotLightComponent* spotLightComponent = new SpotLightComponent(
	    1.0f,
		0.045f,
	    0.0075f,
	    100.0f,
	    1.0f,
	    Color(1.0f, 1.0f, 1.0f, 1.0f),
	    Color(0.05f, 0.05f, 0.05f, 1.0f),
	    4, 
		30 * 3.1415 / 180);

	return spotLightComponent;
}

SpotLightComponent::SpotLightComponent(const float constAtt, const float linAtt, const float quadAtt,
    const float range, const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor,
	float spot, float angleRange)
    : m_AttConst(constAtt)
    , m_AttLin(linAtt)
    , m_AttQuad(quadAtt)
    , m_Range(range)
    , m_AmbientColor(ambientColor)
    , m_DiffuseColor(diffuseColor)
    , m_DiffuseIntensity(diffuseIntensity)
    , m_Spot(spot)
    , m_AngleRange(angleRange)
{
}

SpotLightComponent::~SpotLightComponent()
{
}

JSON::json SpotLightComponent::getJSON() const
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

	j["spot"] = m_Spot;
	j["angleRange"] = m_AngleRange;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void SpotLightComponent::draw()
{
	ImGui::DragFloat("Diffuse Intensity##Spot", &m_DiffuseIntensity);
	ImGui::ColorEdit4("Diffuse Color##Spot", &m_DiffuseColor.x);
	ImGui::ColorEdit4("Ambient Color##Spot", &m_AmbientColor.x);
	ImGui::DragFloat("Constant Attenuation##Spot", &m_AttConst);
	ImGui::DragFloat("Linear Attenuation##Spot", &m_AttLin);
	ImGui::DragFloat("Quadratic Attenuation##Spot", &m_AttQuad);
	ImGui::DragFloat("Range##Spot", &m_Range);
	ImGui::DragFloat("Spot Factor##Spot", &m_Spot);
	ImGui::SliderAngle("Cone Sub-Angle##Spot", &m_AngleRange, 0.0f, 90.0f);
}
#endif // ROOTEX_EDITOR
