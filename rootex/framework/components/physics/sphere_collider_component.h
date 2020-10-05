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
	Ref<btSphereShape> m_SphereShape;
	
	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::SphereColliderComponent;

	SphereColliderComponent(float rad, const String& matName, const Vector3& gravity, bool isMoveable, bool generatesHitEvents);

	float getRadius() const { return m_Radius; }
	virtual const char* getName() const override { return "SphereColliderComponent"; };
	virtual JSON::json getJSON() const override;
	virtual ComponentID getComponentID() const override { return s_ID; }
	
	void setRadius(float r);

/// Interface to change collider properties via editor.
#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR 
};
