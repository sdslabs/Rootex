#pragma once

#include "rigid_body_component.h"

#include "btBulletDynamicsCommon.h"

/// Collider component in the shape of a capsule.
class CapsuleColliderComponent : public RigidBodyComponent
{
	COMPONENT(CapsuleColliderComponent, Category::Physics);

	float m_Radius;
	float m_SideHeight;
	btCapsuleShape* m_CapsuleShape;

public:
	CapsuleColliderComponent(Entity& owner, const JSON::json& data);
	~CapsuleColliderComponent() = default;

	float getSideHeight() const { return m_SideHeight; }
	void setSideHeight(float s);

	float getRadius() const { return m_Radius; }
	void setRadius(float r);

	JSON::json getJSON() const override;
	void draw() override;
};

DECLARE_COMPONENT(CapsuleColliderComponent);
