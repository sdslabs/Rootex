#pragma once

#include "component.h"
#include "components/space/transform_component.h"
#include "components/physics/box_collider_component.h"
#include "components/physics/sphere_collider_component.h"
#include "components/physics/capsule_collider_component.h"

/// Listening component which is used to put sound output to.
/// Useful for marking 3D sound attenuation in moving listeners
class AudioListenerComponent : public Component
{
	DEFINE_COMPONENT(AudioListenerComponent, Category::Audio);

	DEPENDS_ON(TransformComponent);
	SOFT_DEPENDS_ON(CapsuleColliderComponent);
	SOFT_DEPENDS_ON(BoxColliderComponent);
	SOFT_DEPENDS_ON(SphereColliderComponent);

	float m_Volume;

public:
	AudioListenerComponent(Entity& owner, const JSON::json& data);
	~AudioListenerComponent() = default;

	void update();

	Vector3 getPosition();
	Vector3 getUp();
	Vector3 getAt();
	RigidBodyComponent* getCollider();

	JSON::json getJSON() const override;
	void onRemove() override;
	void draw() override;
};
