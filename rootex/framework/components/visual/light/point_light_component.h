#pragma once

#include "common/common.h"
#include "core/renderer/point_light.h"
#include "component.h"
#include "components/space/transform_component.h"

/// Component to apply dynamic point lights to the scene
class PointLightComponent : public Component
{
	DEFINE_COMPONENT(PointLightComponent, Category::Light);
	DEPENDS_ON(TransformComponent);

	PointLight m_PointLight;

public:
	PointLightComponent::PointLightComponent(Entity& owner, const JSON::json& data);
	~PointLightComponent() = default;

	Matrix getAbsoluteTransform() { return getTransformComponent()->getAbsoluteTransform(); }
	const PointLight& getPointLight() const { return m_PointLight; }

	JSON::json getJSON() const override;
	void draw() override;
};
