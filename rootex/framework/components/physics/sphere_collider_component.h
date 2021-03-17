#pragma once

#include "rigid_body_component.h"

#include "btBulletDynamicsCommon.h"

class SphereColliderComponent : public RigidBodyComponent
{
	DEFINE_COMPONENT(SphereColliderComponent);

	float m_Radius;
	btSphereShape* m_SphereShape;

public:
	SphereColliderComponent(
	    float radius,
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
	~SphereColliderComponent() = default;

	float getRadius() const { return m_Radius; }
	void setRadius(float r);

	JSON::json getJSON() const override;
	void draw() override;
};
