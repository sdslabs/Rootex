#pragma once

#include "physics_collider_component.h"

#include "btBulletDynamicsCommon.h"

class BoxColliderComponent : public PhysicsColliderComponent
{
	DEFINE_COMPONENT(BoxColliderComponent);

	Vector3 m_Dimensions;
	btBoxShape* m_BoxShape;

public:
	BoxColliderComponent(
	    const Vector3& dimensions,
	    const Vector3& offset,
	    const PhysicsMaterial& material,
	    const Vector3& angularFactor,
	    const Vector3& gravity,
	    int collisionGroup,
	    int collisionMask,
	    bool isMoveable,
	    bool isKinematic,
	    bool generatesHitEvents,
	    bool isSleepable,
	    bool isCCD);
	~BoxColliderComponent() = default;

	void setDimensions(const Vector3& dimensions);
	Vector3 getDimensions() const { return m_Dimensions; }

	JSON::json getJSON() const override;
	void draw() override;
};
