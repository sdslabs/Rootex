#pragma once

#include "physics_collider_component.h"

#include "btBulletDynamicsCommon.h"

/// Creates sphere shaped collider.
/// Takes box's dimensions and material type as arguments.
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

/// Interface to change collider properties via editor.
#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR 
};
