#pragma once

#include "physics_component.h"

#include "btBulletDynamicsCommon.h"

class SphereComponent : public PhysicsComponent
{
	static Component* Create(const LuaVariable& sphereComponentData);

	friend class EntityFactory;

public:
	
	float m_Radius;
	SphereComponent(float rad, const String& matName);
	//collisionShape.reset(new btSphereShape(radius));

	// calculate absolute mass from specificGravity
	//volume = (4.f / 3.f) * 3.14 * radius * radius * radius;
};
