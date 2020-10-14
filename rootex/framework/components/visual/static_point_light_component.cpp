#include "static_point_light_component.h"

Component* StaticPointLightComponent::Create(const JSON::json& componentData)
{
	StaticPointLightComponent* staticPointLightComponent = new StaticPointLightComponent(
	    componentData.value("attConst", 0.045f),
	    componentData.value("attLin", 1.0f),
	    componentData.value("attQuad", 0.0075f),
	    componentData.value("range", 10.0f),
	    componentData.value("diffuseIntensity", 1.0f),
	    componentData.value("diffuseColor", Color(1.0f, 1.0f, 1.0f, 1.0f)),
	    componentData.value("ambientColor", Color(0.5f, 0.5f, 0.5f, 1.0f)));
	return staticPointLightComponent;
}

Component* StaticPointLightComponent::CreateDefault()
{
	StaticPointLightComponent* staticPointLightComponent = new StaticPointLightComponent(
	    0.045f,
	    1.0f,
	    0.0075f,
	    10.0f,
	    1.0f,
	    Color(1.0f, 1.0f, 1.0f, 1.0f),
	    Color(0.5f, 0.5f, 0.5f, 1.0f));

	return staticPointLightComponent;
}

StaticPointLightComponent::StaticPointLightComponent(const float constAtt, const float linAtt, const float quadAtt, const float range, const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor)
    : PointLightComponent(constAtt, linAtt, quadAtt, range, diffuseIntensity, diffuseColor, ambientColor)
{
}

JSON::json StaticPointLightComponent::getJSON() const
{
	JSON::json j = PointLightComponent::getJSON();
	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void StaticPointLightComponent::draw()
{
	PointLightComponent::draw();
}
#endif // ROOTEX_EDITOR
