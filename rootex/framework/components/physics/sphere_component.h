#pragma once

#include "physics_component.h"

#include "btBulletDynamicsCommon.h"

class SphereComponent : public PhysicsComponent
{
	static Component* Create(const LuaVariable& componentData);

	friend class EntityFactory;

	float radius;

	btSphereShape* const collisionShape = new btSphereShape(radius);

	// calculate absolute mass from specificGravity
	volume = (4.f / 3.f) * 3.14 * radius * radius * radius;
	
}