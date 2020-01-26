#pragma once

#include "component.h"

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
};
