#pragma once

#include "physics_component.h"

#include "btBulletDynamicsCommon.h"

class SphereColliderComponent : public PhysicsColliderComponent
{
	static Component* Create(const JSON::json& sphereComponentData);
	static Component* CreateDefault();

	float m_Radius;
	
	friend class EntityFactory;

public:
	SphereColliderComponent(float rad, const String& matName);

	float getRadius() const { return m_Radius; }

	virtual String getName() const override { return "SphereColliderComponent"; };
	virtual JSON::json getJSON() const override;
};
