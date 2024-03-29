#pragma once

#include "rigid_body_component.h"

#include "btBulletDynamicsCommon.h"

/// Collider component in the shape of a box.
class BoxColliderComponent : public RigidBodyComponent
{
	COMPONENT(BoxColliderComponent, Category::Physics);

	Vector3 m_Dimensions;
	btBoxShape* m_BoxShape;

public:
	BoxColliderComponent(Entity& owner, const JSON::json& data);
	~BoxColliderComponent() = default;

	void setDimensions(const Vector3& dimensions);
	Vector3 getDimensions() const { return m_Dimensions; }

	JSON::json getJSON() const override;
	void draw() override;
};

DECLARE_COMPONENT(BoxColliderComponent);
