#pragma once

#include "component.h"

class FogComponent : public Component
{
	COMPONENT(FogComponent, Category::Effect);

	float m_Near;
	float m_Far;
	Color m_Color;

public:
	FogComponent(Entity& owner, const JSON::json& data);
	FogComponent() = default;

	Color getColor() const { return m_Color; }
	float getNearDistance() const { return m_Near; }
	float getFarDistance() const { return m_Far; }

	JSON::json getJSON() const override;
	void draw() override;
};

DECLARE_COMPONENT(FogComponent);
