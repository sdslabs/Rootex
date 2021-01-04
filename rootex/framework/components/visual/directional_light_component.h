#pragma once

#include "component.h"
#include "common/common.h"

#include "components/transform_component.h"
#include "core/renderer/directional_light.h"

/// Component to apply a dynamic directional light to the scene, only the first created instance is used in case of multiple such components
class DirectionalLightComponent : public Component
{
	DEPENDS_ON(TransformComponent);

	static Component* Create(const JSON::json& componentData);

	DirectionalLightComponent(const float diffuseIntensity,
	    const Color& diffuseColor, const Color& ambientColor);
	DirectionalLightComponent(DirectionalLightComponent&) = delete;
	~DirectionalLightComponent() = default;

	friend class ECSFactory;

	DirectionalLight m_DirectionalLight;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::DirectionalLightComponent;
	
	Vector3 getDirection() const { return m_TransformComponent->getAbsoluteTransform().Forward(); }
	const DirectionalLight& getDirectionalLight() const { return m_DirectionalLight; }

	virtual const char* getName() const override { return "DirectionalLightComponent"; }
	ComponentID getComponentID() const override { return s_ID; }

	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
