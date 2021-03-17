#pragma once

#include "component.h"
#include "common/common.h"
#include "components/space/transform_component.h"
#include "core/renderer/directional_light.h"

/// Component to apply a dynamic directional light to the scene, only the first created instance is used in case of multiple such components
class DirectionalLightComponent : public Component
{
	DEFINE_COMPONENT(DirectionalLightComponent);
	DEPENDS_ON(TransformComponent);

	DirectionalLight m_DirectionalLight;

public:
	DirectionalLightComponent(const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor);
	~DirectionalLightComponent() = default;

	Vector3 getDirection() const { return m_TransformComponent->getAbsoluteTransform().Forward(); }
	const DirectionalLight& getDirectionalLight() const { return m_DirectionalLight; }

	JSON::json getJSON() const override;
	void draw() override;
};
