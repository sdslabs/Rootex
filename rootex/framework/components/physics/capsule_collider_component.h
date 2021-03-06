#pragma once

#include "physics_collider_component.h"

#include "btBulletDynamicsCommon.h"

class CapsuleColliderComponent : public PhysicsColliderComponent
{
	static Component* Create(const JSON::json& capsuleComponentData);

	float m_Radius;
	float m_SideHeight;
	btCapsuleShape* m_CapsuleShape;

	friend class ECSFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::CapsuleColliderComponent;

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

	float getSideHeight() const { return m_SideHeight; }
	void setSideHeight(float s);

	float getRadius() const { return m_Radius; }
	void setRadius(float r);

	virtual const char* getName() const override { return "CapsuleColliderComponent"; };
	virtual JSON::json getJSON() const override;
	virtual ComponentID getComponentID() const override { return s_ID; }

	void draw() override;
};
