#pragma once

#include "common/common.h"
#include "component.h"
#include "components/space/transform_component.h"

class GodRaysComponent : public Component
{
	COMPONENT(GodRaysComponent, Category::Light);
	DEPENDS_ON(TransformComponent);

	int m_NumSamples;
	float m_Density;
	float m_Weight;
	float m_Decay;
	float m_Exposure;

public:
	GodRaysComponent::GodRaysComponent(Entity& owner, const JSON::json& data);
	~GodRaysComponent() = default;

	int getNumSamples() { return m_NumSamples; };
	float getDensity() { return m_Density; };
	float getWeight() { return m_Weight; };
	float getDecay() { return m_Decay; };
	float getExposure() { return m_Exposure; };

	Vector3 getScreenSpacePosition();

	void render();

	JSON::json getJSON() const override;
	void draw() override;
};

DECLARE_COMPONENT(GodRaysComponent);
