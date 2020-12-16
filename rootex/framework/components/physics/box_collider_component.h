#pragma once

#include "physics_collider_component.h"

#include "btBulletDynamicsCommon.h"

class BoxColliderComponent : public PhysicsColliderComponent
{
	static Component* Create(const JSON::json& boxComponentData);
	
	Vector3 m_Dimensions;
	Ref<btBoxShape> m_BoxShape;

	friend class ECSFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::BoxColliderComponent;

	BoxColliderComponent(const Vector3& dimensions, const PhysicsMaterial& material, const Vector3& gravity, bool isMoveable, bool isKinematic, bool generatesHitEvents);

	Vector3 getDimensions() const { return m_Dimensions; }
	virtual const char* getName() const override { return "BoxColliderComponent"; };
	virtual JSON::json getJSON() const override;
	virtual ComponentID getComponentID() const override { return s_ID; }

	void setDimensions(const Vector3& dimensions);

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR 
};
