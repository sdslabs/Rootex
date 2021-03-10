#include "capsule_collider_component.h"
#include "framework/systems/physics_system.h"

Component* CapsuleColliderComponent::Create(const JSON::json& capsuleComponentData)
{
	CapsuleColliderComponent* component = new CapsuleColliderComponent(
	    capsuleComponentData.value("radius", 0.5f),
	    capsuleComponentData.value("sideHeight", 1.0f),
	    capsuleComponentData.value("offset", Vector3(0.0f, 0.0f, 0.0f)),
	    capsuleComponentData.value("material", PhysicsMaterial::Air),
	    capsuleComponentData.value("angularFactor", Vector3::One),
	    capsuleComponentData.value("gravity", Vector3(0.0f, -9.8f, 0.0f)),
	    capsuleComponentData.value("collisionGroup", (int)CollisionMask::All),
	    capsuleComponentData.value("collisionMask", (int)CollisionMask::All),
	    capsuleComponentData.value("isMoveable", false),
	    capsuleComponentData.value("isKinematic", false),
	    capsuleComponentData.value("isGeneratesHitEvents", false),
	    capsuleComponentData.value("isSleepable", true),
	    capsuleComponentData.value("isCCD", false));
	return component;
}

CapsuleColliderComponent::CapsuleColliderComponent(
    float radius,
    float sideHeight,
    const Vector3& offset,
    const PhysicsMaterial& material,
    const Vector3& angularFactor,
    const Vector3& gravity,
    int collisionGroup,
    int collisionMask,
    bool isMoveable,
    bool isKinematic,
    bool generatesHitEvents,
    bool isSleepable,
    bool isCCD)
    : PhysicsColliderComponent(
        material,
        DirectX::XM_PI * radius * radius * ((4.0f / 3.0f) * radius + sideHeight),
        offset,
        gravity,
        angularFactor,
        collisionGroup,
        collisionMask,
        isMoveable,
        isKinematic,
        generatesHitEvents,
        isSleepable,
        isCCD,
        Ref<btCapsuleShape>(new btCapsuleShape(radius, sideHeight)))
    , m_Radius(radius)
    , m_SideHeight(sideHeight)
{
	m_CapsuleShape = (btCapsuleShape*)m_CollisionShape.get();
}

void CapsuleColliderComponent::setSideHeight(float s)
{
	m_SideHeight = s;
	setupData();
	m_CapsuleShape = (btCapsuleShape*)m_CollisionShape.get();
}

void CapsuleColliderComponent::setRadius(float r)
{
	m_Radius = r;
	m_CollisionShape.reset(new btCapsuleShape(m_Radius, m_SideHeight));
	m_CapsuleShape = (btCapsuleShape*)m_CollisionShape.get();
	setupData();
}

JSON::json CapsuleColliderComponent::getJSON() const
{
	JSON::json& j = PhysicsColliderComponent::getJSON();

	j["radius"] = m_Radius;
	j["sideHeight"] = m_SideHeight;

	return j;
}

void CapsuleColliderComponent::draw()
{
	PhysicsColliderComponent::draw();

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
