#pragma once

#include "physics_component.h"

#include "btBulletDynamicsCommon.h"

class BoxColliderComponent : public PhysicsColliderComponent
{
	static Component* Create(const JSON::json& boxComponentData);
	static Component* CreateDefault();

	Vector3 m_Dimensions;

	friend class EntityFactory;

public:
	BoxColliderComponent(const Vector3 dimensions, const String& matName);

	Vector3 getDimensions() const { return m_Dimensions; }

	virtual String getName() const override { return "BoxColliderComponent"; };
	virtual JSON::json getJSON() const override;
};
