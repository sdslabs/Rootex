#pragma once

#include "physics_component.h"

#include "btBulletDynamicsCommon.h"

class BoxComponent : public PhysicsComponent
{
	static Component* Create(const JSON::json& boxComponentData);
	static Component* CreateDefault();

	Vector3 m_Dimensions;

	friend class EntityFactory;

public:
	BoxComponent(const Vector3 dimensions, const String& matName);

	Vector3 getDimensions() const { return m_Dimensions; }

	virtual String getName() const override { return "BoxComponent"; };
	virtual JSON::json getJSON() const override;
};
