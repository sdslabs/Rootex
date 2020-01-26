#include "physics_component.h"

#include "entity.h"

Component* PhysicsComponent::Create(const LuaVariable& componentData)
{
	PhysicsComponent* component = new PhysicsComponent();

	return component;
}

PhysicsComponent::MaterialData::MaterialData(float restitution, float friction)
{
	m_Restitution = restitution;
	m_Friction = friction;
}
