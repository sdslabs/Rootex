#include "sphere_collider_component.h"
#include "framework/systems/physics_system.h"

DEFINE_COMPONENT(SphereColliderComponent);

SphereColliderComponent::SphereColliderComponent(Entity& owner, const JSON::json& data)
    : RigidBodyComponent(
        owner,
        data.value("material", PhysicsMaterial::Air),
        (4.0f / 3.0f) * DirectX::XM_PI * data.value("radius", 1.0f) * data.value("radius", 1.0f) * data.value("radius", 1.0f),
        data.value("offset", Vector3(0.0f, 0.0f, 0.0f)),
        data.value("gravity", Vector3(0.0f, -9.8f, 0.0f)),
        data.value("angularFactor", Vector3::One),
        data.value("collisionGroup", (int)CollisionMask::All),
        data.value("collisionMask", (int)CollisionMask::All),
        data.value("isMoveable", false),
        data.value("isKinematic", false),
        data.value("isGeneratesHitEvents", false),
        data.value("isSleepable", true),
        data.value("isCCD", false),
        Ref<btSphereShape>(new btSphereShape(data.value("radius", 1.0f))))
    , m_Radius(data.value("radius", 1.0f))
{
	m_SphereShape = (btSphereShape*)m_CollisionShape.get();
}

JSON::json SphereColliderComponent::getJSON() const
{
	JSON::json& j = RigidBodyComponent::getJSON();

	j["radius"] = m_Radius;

	return j;
}

void SphereColliderComponent::setRadius(float r)
{
	detachCollisionObject();
	m_Radius = r;
	m_CollisionShape.reset(new btSphereShape(m_Radius));
	m_SphereShape = (btSphereShape*)m_CollisionShape.get();
	m_Body->setCollisionShape(m_SphereShape);
	attachCollisionObject();
}

void SphereColliderComponent::draw()
{
	RigidBodyComponent::draw();

	if (ImGui::DragFloat("##Radius", &m_Radius, 0.01f))
	{
		setRadius(m_Radius);
	}
	ImGui::SameLine();
	if (ImGui::Button("Radius"))
	{
		setRadius(1.0f);
	}
}
