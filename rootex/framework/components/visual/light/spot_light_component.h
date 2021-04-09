#pragma once

#include "component.h"
#include "common/common.h"

#include "components/space/transform_component.h"
#include "core/renderer/spot_light.h"

/// Component to apply dynamic spot lights to the scene
class SpotLightComponent : public Component
{
	DEFINE_COMPONENT(SpotLightComponent, Category::Light);
	DEPENDS_ON(TransformComponent);

	SpotLight m_SpotLight;

public:
	SpotLightComponent::SpotLightComponent(Entity& owner, const JSON::json& data);
	~SpotLightComponent() = default;

	Matrix getAbsoluteTransform() { return getTransformComponent()->getAbsoluteTransform(); }
	const SpotLight& getSpotLight() const { return m_SpotLight; }

	JSON::json getJSON() const override;
	void draw() override;
};
