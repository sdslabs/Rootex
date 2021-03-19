#include "static_point_light_component.h"

#include "systems/render_system.h"

Ptr<Component> StaticPointLightComponent::Create(const JSON::json& componentData)
{
	return std::make_unique<StaticPointLightComponent>(
	    componentData.value("attConst", 0.045f),
	    componentData.value("attLin", 1.0f),
	    componentData.value("attQuad", 0.0075f),
	    componentData.value("range", 10.0f),
	    componentData.value("diffuseIntensity", 1.0f),
	    componentData.value("diffuseColor", Color(1.0f, 1.0f, 1.0f, 1.0f)),
	    componentData.value("ambientColor", Color(0.5f, 0.5f, 0.5f, 1.0f)));
}

StaticPointLightComponent::StaticPointLightComponent(
    const float constAtt,
    const float linAtt,
    const float quadAtt,
    const float range,
    const float diffuseIntensity,
    const Color& diffuseColor,
    const Color& ambientColor)
    : PointLightComponent(constAtt, linAtt, quadAtt, range, diffuseIntensity, diffuseColor, ambientColor)
{
}

void StaticPointLightComponent::draw()
{
	if (ImGui::Button("Update Lights"))
	{
		RenderSystem::GetSingleton()->updateStaticLights();
	}
	PointLightComponent::draw();
}
