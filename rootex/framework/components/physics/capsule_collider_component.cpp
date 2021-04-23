#include "capsule_collider_component.h"
#include "framework/systems/physics_system.h"

DEFINE_COMPONENT(CapsuleColliderComponent);

CapsuleColliderComponent::CapsuleColliderComponent(Entity& owner, const JSON::json& data)
    : RigidBodyComponent(
        owner,
        data.value("material", PhysicsMaterial::Air),
        DirectX::XM_PI * data.value("radius", 0.5f) * data.value("radius", 0.5f) * ((4.0f / 3.0f) * data.value("radius", 0.5f) + data.value("sideHeight", 1.0f)),
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
        Ref<btCapsuleShape>(new btCapsuleShape(data.value("radius", 0.5f), data.value("sideHeight", 1.0f))))
    , m_Radius(data.value("radius", 0.5f))
    , m_SideHeight(data.value("sideHeight", 1.0f))
{
	m_CapsuleShape = (btCapsuleShape*)m_CollisionShape.get();
}

void CapsuleColliderComponent::setSideHeight(float s)
{
	detachCollisionObject();
	m_SideHeight = s;
	m_CollisionShape.reset(new btCapsuleShape(m_Radius, m_SideHeight));
	m_CapsuleShape = (btCapsuleShape*)m_CollisionShape.get();
	m_Body->setCollisionShape(m_CapsuleShape);
	attachCollisionObject();
}

void CapsuleColliderComponent::setRadius(float r)
{
	detachCollisionObject();
	m_Radius = r;
	m_CollisionShape.reset(new btCapsuleShape(m_Radius, m_SideHeight));
	m_CapsuleShape = (btCapsuleShape*)m_CollisionShape.get();
	m_Body->setCollisionShape(m_CapsuleShape);
	attachCollisionObject();
}

JSON::json CapsuleColliderComponent::getJSON() const
{
	JSON::json& j = RigidBodyComponent::getJSON();

	j["radius"] = m_Radius;
	j["sideHeight"] = m_SideHeight;

	return j;
}

void CapsuleColliderComponent::draw()
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
	if (ImGui::DragFloat("##SideHeight", &m_SideHeight, 0.01f))
	{
		setSideHeight(m_SideHeight);
	}
	ImGui::SameLine();
	if (ImGui::Button("Side Height"))
	{
		setSideHeight(1.0f);
	}
}
