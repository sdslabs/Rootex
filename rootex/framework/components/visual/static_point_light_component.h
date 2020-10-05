#pragma once

#include "common/common.h"
#include "component.h"

#include "point_light_component.h"

/// Component to apply static point lights to the scene. These type of lights are rendered in larger
/// numbers with the limitation that they are static i.e. movement of these lights will not show up
/// inside the final render.
class StaticPointLightComponent : public PointLightComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	StaticPointLightComponent::StaticPointLightComponent(const float constAtt, const float linAtt, const float quadAtt,
	    const float range, const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor);
	StaticPointLightComponent(StaticPointLightComponent&) = delete;
	~StaticPointLightComponent() = default;

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::StaticPointLightComponent;

	virtual const char* getName() const override { return "StaticPointLightComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
