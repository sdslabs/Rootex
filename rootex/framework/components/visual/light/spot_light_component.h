#pragma once

#include "component.h"
#include "common/common.h"

#include "components/space/transform_component.h"
#include "core/renderer/spot_light.h"

/// Component to apply dynamic spot lights to the scene
class SpotLightComponent : public Component
{
	DEFINE_COMPONENT(SpotLightComponent);
	DEPENDS_ON(TransformComponent);

	SpotLight m_SpotLight;

public:
	SpotLightComponent::SpotLightComponent(const float constAtt, const float linAtt, const float quadAtt,
	    const float range, const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor,
	    float spot, float angleRange);
	~SpotLightComponent() = default;

	Matrix getAbsoluteTransform() const { return m_TransformComponent->getAbsoluteTransform(); }
	const SpotLight& getSpotLight() const { return m_SpotLight; }

	JSON::json getJSON() const override;
	void draw() override;
};
