#pragma once

#include "common/common.h"
#include "framework/component.h"
#include "framework/components/space/transform_component.h"

class SpringArmComponent : public Component
{
	COMPONENT(SpringArmComponent, Category::Game);
	DEPENDS_ON(TransformComponent);

	Vector3 m_DesiredLocalPosition;
	float m_Lerp;

public:
	SpringArmComponent(Entity& owner, const JSON::json& data);
	~SpringArmComponent() = default;

	Vector3 getDesiredLocalPosition() { return m_DesiredLocalPosition; }
	void setDesiredLocalPosition(const Vector3& position) { m_DesiredLocalPosition = position; }
	float getLerp() { return m_Lerp; }
	void setLerp(const float& value) { m_Lerp = value; }

	virtual JSON::json getJSON() const override;
	void draw() override;
};

DECLARE_COMPONENT(SpringArmComponent);
