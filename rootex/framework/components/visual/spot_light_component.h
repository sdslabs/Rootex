#pragma once

#include "component.h"
#include "common/common.h"

#include "components/transform_component.h"
#include "core/renderer/spot_light.h"

/// Component to apply dynamic spot lights to the scene
class SpotLightComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	SpotLightComponent::SpotLightComponent(const float constAtt, const float linAtt, const float quadAtt,
	    const float range, const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor,
		float spot, float angleRange);
	SpotLightComponent(SpotLightComponent&) = delete;
	~SpotLightComponent() = default;

	friend class EntityFactory;

	SpotLight m_SpotLight;

	TransformComponent* m_TransformComponent;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::SpotLightComponent;
	
	bool setup() override;

	const SpotLight& getSpotLight() const { return m_SpotLight; }

	virtual const char* getName() const override { return "SpotLightComponent"; }
	ComponentID getComponentID() const override { return s_ID; }

	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
