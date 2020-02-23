#include "point_light_component.h"

Component* PointLightComponent::Create(const LuaVariable& componentData)
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
