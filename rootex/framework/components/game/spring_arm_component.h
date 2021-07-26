#pragma once

#include "component.h"
#include "components/space/transform_component.h"
#include "common/common.h"
#include "framework/component.h"
#include "framework/components/space/transform_component.h"
#include "framework/components/visual/model/animated_model_component.h"
#include "framework/components/physics/capsule_collider_component.h"
#include "core/state_manager.h"

class SpringArmComponent : public Component
{
	COMPONENT(SpringArmComponent, Category::Game);
	DEPENDS_ON(TransformComponent);

	Vector3 m_DesiredLocalPosition;

public:
	SpringArmComponent(Entity& owner, const JSON::json& data);
	~SpringArmComponent() = default;

	float m_Lerp;

	virtual JSON::json getJSON() const override;
	bool setupData() override;

	Vector3 getDesiredLocalPosition() { return m_DesiredLocalPosition; }
	void setDesiredLocalPosition(const Vector3& position) { m_DesiredLocalPosition = position; }

	void draw();
};

DECLARE_COMPONENT(SpringArmComponent);
