#pragma once

#include "rigid_body_component.h"

#include "btBulletDynamicsCommon.h"

class SphereColliderComponent : public RigidBodyComponent
{
	COMPONENT(SphereColliderComponent, Category::Physics);

	float m_Radius;
	btSphereShape* m_SphereShape;

public:
	SphereColliderComponent(Entity& owner, const JSON::json& data);
	~SphereColliderComponent() = default;

	float getRadius() const { return m_Radius; }
	void setRadius(float r);

	JSON::json getJSON() const override;
	void draw() override;
};

DECLARE_COMPONENT(SphereColliderComponent);
