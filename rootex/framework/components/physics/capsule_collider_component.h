#pragma once

#include "rigid_body_component.h"

#include "btBulletDynamicsCommon.h"

class CapsuleColliderComponent : public RigidBodyComponent
{
	DEFINE_COMPONENT(CapsuleColliderComponent);

	float m_Radius;
	float m_SideHeight;
	btCapsuleShape* m_CapsuleShape;

public:
	CapsuleColliderComponent(
	    float radius,
	    float sideHeight,
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
	~CapsuleColliderComponent() = default;

	float getSideHeight() const { return m_SideHeight; }
	void setSideHeight(float s);

	float getRadius() const { return m_Radius; }
	void setRadius(float r);

	JSON::json getJSON() const override;
	void draw() override;
};
