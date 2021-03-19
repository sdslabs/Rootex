#pragma once

#include "component.h"

class FogComponent : public Component
{
	DEFINE_COMPONENT(FogComponent);

	float m_Near;
	float m_Far;
	Color m_Color;

public:
	FogComponent(float nearDistance, float farDistance, const Color& color);
	FogComponent() = default;

	Color getColor() const { return m_Color; }
	float getNearDistance() const { return m_Near; }
	float getFarDistance() const { return m_Far; }

	JSON::json getJSON() const override;
	void draw() override;
};
