#pragma once

#include "component.h"
#include "common/common.h"

#include "core/renderer/directional_light.h"

/// Component to apply a dynamic directional light to the scene, only the first created instance is used in case of multiple such components
class DirectionalLightComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	DirectionalLightComponent(const Vector3& direction, const float diffuseIntensity,
	    const Color& diffuseColor, const Color& ambientColor);
	DirectionalLightComponent(DirectionalLightComponent&) = delete;
	~DirectionalLightComponent() = default;

	friend class EntityFactory;

	DirectionalLight m_DirectionalLight;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::DirectionalLightComponent;
	
	const DirectionalLight& getDirectionalLight() const { return m_DirectionalLight; }

	virtual String getName() const override { return "DirectionalLightComponent"; }
	ComponentID getComponentID() const override { return s_ID; }

	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
