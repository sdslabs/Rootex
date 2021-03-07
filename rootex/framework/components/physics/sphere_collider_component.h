#pragma once

#include "physics_collider_component.h"

#include "btBulletDynamicsCommon.h"

class SphereColliderComponent : public PhysicsColliderComponent
{
	static Component* Create(const JSON::json& sphereComponentData);

	float m_Radius;
	btSphereShape* m_SphereShape;

	friend class ECSFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::SphereColliderComponent;

	SphereColliderComponent(
	    float radius,
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

	float getRadius() const { return m_Radius; }
	void setRadius(float r);

	virtual const char* getName() const override { return "SphereColliderComponent"; };
	virtual JSON::json getJSON() const override;
	virtual ComponentID getComponentID() const override { return s_ID; }

	void draw() override;
};
