#pragma once

#include "common/common.h"
#include "component.h"
#include "components/transform_component.h"
#include "core/renderer/point_light.h"

/// Component to apply dynamic point lights to the scene
class PointLightComponent : public Component
{
	DEPENDS_ON(TransformComponent);

	static Component* Create(const JSON::json& componentData);

	PointLight m_PointLight;

protected:
	PointLightComponent::PointLightComponent(const float constAtt, const float linAtt, const float quadAtt,
	    const float range, const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor);
	PointLightComponent(PointLightComponent&) = delete;
	~PointLightComponent() = default;

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::PointLightComponent;

	Matrix getAbsoluteTransform() const { return m_TransformComponent->getAbsoluteTransform(); }
	const PointLight& getPointLight() const { return m_PointLight; }

	virtual const char* getName() const override { return "PointLightComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
