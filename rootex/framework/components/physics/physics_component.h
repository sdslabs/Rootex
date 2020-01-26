#pragma once

#include "component.h"

#include"btBulletDynamicsCommon.h"

class PhysicsComponent : public Component
{
	static Component* Create(const LuaVariable& componentData);

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::PhysicsComponent;

	ComponentID getComponentID() const { return s_ID; }

	struct MaterialData
	{
		float m_Restitution;
		float m_Friction;

		MaterialData(float restitution, float friction);
	};

	static btTransform Mat4x4_to_btTransform(Matrix const& mat);

	static Matrix btTransform_to_Mat4x4(btTransform const& trans);

};
