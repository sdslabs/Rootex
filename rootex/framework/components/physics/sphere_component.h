#pragma once

#include "physics_component.h"

#include "btBulletDynamicsCommon.h"

class SphereComponent : public PhysicsComponent
{
	static Component* Create(const LuaVariable& sphereComponentData);
	static Component* CreateDefault();

	float m_Radius;
	
	friend class EntityFactory;

public:
	SphereComponent(float rad, const String& matName);

	float getRadius() const { return m_Radius; }

	virtual String getName() const override { return "SphereComponent"; };
};
