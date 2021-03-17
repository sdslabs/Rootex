#pragma once

#include "common/common.h"
#include "component.h"

#include "point_light_component.h"

/// Component to apply static point lights to the scene. These type of lights are rendered in larger
/// numbers with the limitation that they are static i.e. movement of these lights will not show up
/// inside the final render.
class StaticPointLightComponent : public PointLightComponent
{
	DEFINE_COMPONENT(StaticPointLightComponent);

public:
	StaticPointLightComponent::StaticPointLightComponent(
	    const float constAtt,
	    const float linAtt,
	    const float quadAtt,
	    const float range,
	    const float diffuseIntensity,
	    const Color& diffuseColor,
	    const Color& ambientColor);
	~StaticPointLightComponent() = default;

	void draw() override;
};
