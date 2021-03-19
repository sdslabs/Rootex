#pragma once

#include "common/common.h"
#include "core/renderer/point_light.h"
#include "component.h"
#include "components/space/transform_component.h"

/// Component to apply dynamic point lights to the scene
class PointLightComponent : public Component
{
	DEFINE_COMPONENT(PointLightComponent);
	DEPENDS_ON(TransformComponent);

	PointLight m_PointLight;

public:
	PointLightComponent::PointLightComponent(const float constAtt, const float linAtt, const float quadAtt,
	    const float range, const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor);
	~PointLightComponent() = default;

	Matrix getAbsoluteTransform() const { return m_TransformComponent->getAbsoluteTransform(); }
	const PointLight& getPointLight() const { return m_PointLight; }

	JSON::json getJSON() const override;
	void draw() override;
};
