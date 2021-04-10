#pragma once

#include "component.h"
#include "common/common.h"
#include "components/space/transform_component.h"
#include "core/renderer/directional_light.h"

/// Component to apply a dynamic directional light to the scene, only the first created instance is used in case of multiple such components
class DirectionalLightComponent : public Component
{
	COMPONENT(DirectionalLightComponent, Category::Light);
	DEPENDS_ON(TransformComponent);

	DirectionalLight m_DirectionalLight;

public:
	DirectionalLightComponent(Entity& owner, const JSON::json& data);
	~DirectionalLightComponent() = default;

	Vector3 getDirection() { return getTransformComponent()->getAbsoluteTransform().Forward(); }
	const DirectionalLight& getDirectionalLight() const { return m_DirectionalLight; }

	JSON::json getJSON() const override;
	void draw() override;
};

DECLARE_COMPONENT(DirectionalLightComponent);
