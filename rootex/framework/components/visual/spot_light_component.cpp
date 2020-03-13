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
		30);

	return spotLightComponent;
}

SpotLightComponent::SpotLightComponent(const float constAtt, const float linAtt, const float quadAtt,
    const float range, const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor,
	float spot, float angleRange)
    : m_constAtt(constAtt)
    , m_linAtt(linAtt)
    , m_quadAtt(quadAtt)
    , m_range(range)
    , m_ambientColor(ambientColor)
    , m_diffuseColor(diffuseColor)
    , m_diffuseIntensity(diffuseIntensity)
    , m_spot(spot)
{
	m_angleRange = cos(angleRange * 3.1415 / 180);
}

SpotLightComponent::~SpotLightComponent()
{
}

JSON::json SpotLightComponent::getJSON() const
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

	j["spot"] = m_spot;
	j["angleRange"] = m_angleRange;

	return j;
}
