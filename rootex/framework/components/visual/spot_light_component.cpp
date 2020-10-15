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
	
	j["diffuseColor"]["r"] = m_SpotLight.diffuseColor.x;
	j["diffuseColor"]["g"] = m_SpotLight.diffuseColor.y;
	j["diffuseColor"]["b"] = m_SpotLight.diffuseColor.z;
	j["diffuseColor"]["a"] = m_SpotLight.diffuseColor.w;

	j["ambientColor"]["r"] = m_SpotLight.ambientColor.x;
	j["ambientColor"]["g"] = m_SpotLight.ambientColor.y;
	j["ambientColor"]["b"] = m_SpotLight.ambientColor.z;
	j["ambientColor"]["a"] = m_SpotLight.ambientColor.w;

	j["spot"] = m_SpotLight.spot;
	j["angleRange"] = m_SpotLight.range;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void SpotLightComponent::draw()
{
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
#endif // ROOTEX_EDITOR
