#pragma once

#include "physics_collider_component.h"

#include "btBulletDynamicsCommon.h"

/// Creates sphere shaped collider.
/// Takes sphere's radius and material type as arguments. 
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

/// Interface to change collider properties via editor.
#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR 
};
