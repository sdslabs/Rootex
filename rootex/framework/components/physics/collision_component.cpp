#include "collision_component.h"

#include "systems/physics_system.h"

void CollisionComponent::detachCollisionObject()
{
	PhysicsSystem::GetSingleton()->removeCollisionObject(m_CollisionObject.get());
}

void CollisionComponent::attachCollisionObject()
{
	PhysicsSystem::GetSingleton()->addCollisionObject(m_CollisionObject.get(), m_CollisionGroup, m_CollisionMask);
}

CollisionComponent::CollisionComponent(Entity& owner, int collisionGroup, int collisionMask)
    : Component(owner)
    , m_CollisionGroup(collisionGroup)
    , m_CollisionMask(collisionMask)
{
}

void CollisionComponent::handleHit(Hit* h)
{
}

void CollisionComponent::onRemove()
{
	if (m_CollisionObject)
	{
		PhysicsSystem::GetSingleton()->removeCollisionObject(m_CollisionObject.get());
		m_CollisionObject.reset();
	}
}

JSON::json CollisionComponent::getJSON() const
{
	JSON::json j;

	j["collisionGroup"] = m_CollisionGroup;
	j["collisionMask"] = m_CollisionMask;

	return j;
}

void CollisionComponent::draw()
{
	if (ImGui::TreeNodeEx("Collision Group"))
	{
		displayCollisionLayers(m_CollisionGroup);
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Collision Mask"))
	{
		displayCollisionLayers(m_CollisionMask);
		ImGui::TreePop();
	}
}

void CollisionComponent::displayCollisionLayers(unsigned int& collision)
{
	ImGui::CheckboxFlags("Player", &collision, (int)CollisionMask::Player);
	ImGui::CheckboxFlags("Enemy", &collision, (int)CollisionMask::Enemy);
	ImGui::CheckboxFlags("Architecture", &collision, (int)CollisionMask::Architecture);
	ImGui::CheckboxFlags("TriggerVolume", &collision, (int)CollisionMask::TriggerVolume);
	ImGui::CheckboxFlags("All", &collision, (int)CollisionMask::All);
}
