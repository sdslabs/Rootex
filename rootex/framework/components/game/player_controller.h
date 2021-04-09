#pragma once

#include "common/common.h"
#include "framework/component.h"
#include "framework/components/space/transform_component.h"
#include "framework/components/visual/model/animated_model_component.h"
#include "framework/components/physics/capsule_collider_component.h"

class PlayerController : public Component
{
	DEFINE_COMPONENT(PlayerController, Category::Game);
	DEPENDS_ON(TransformComponent);
	DEPENDS_ON(AnimatedModelComponent);
	DEPENDS_ON(CapsuleColliderComponent);

public:
	PlayerController(Entity& owner, const JSON::json& data);
	~PlayerController() = default;

	bool setupData() override;
	JSON::json getJSON() const override;
	void draw() override;
};
