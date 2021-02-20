#include "sphere_collider_component.h"
#include "framework/systems/physics_system.h"

Component* SphereColliderComponent::Create(const JSON::json& sphereComponentData)
{
	SphereColliderComponent* component = new SphereColliderComponent(
		sphereComponentData.value("radius", 1.0f), 
		sphereComponentData.value("material", PhysicsMaterial::Air),
	    sphereComponentData.value("angularFactor", Vector3::One),
	    sphereComponentData.value("gravity", Vector3(0.0f, -9.8f, 0.0f)),
	    sphereComponentData.value("collisionGroup", (int)CollisionMask::All),
	    sphereComponentData.value("collisionMask", (int)CollisionMask::All),
	    sphereComponentData.value("isMoveable", false),
	    sphereComponentData.value("isKinematic", false),
	    sphereComponentData.value("isGeneratesHitEvents", false),
	    sphereComponentData.value("isSleepable", true));
	return component;
}

SphereColliderComponent::SphereColliderComponent(
	float radius, 
	const PhysicsMaterial& material, 
	const Vector3& angularFactor, 
	const Vector3& gravity, 
	int collisionGroup, 
	int collisionMask, 
	bool isMoveable, 
	bool isKinematic, 
	bool generatesHitEvents,
    bool isSleepable)
    : PhysicsColliderComponent(
		material, 
		(4.0f / 3.0f) * DirectX::XM_PI * radius * radius * radius, 
		gravity, 
		angularFactor, 
		collisionGroup, 
		collisionMask, 
		isMoveable, 
		isKinematic, 
		generatesHitEvents,
        isSleepable,
		Ref<btSphereShape>(new btSphereShape(radius)))
    , m_Radius(radius)
{
	m_SphereShape = std::dynamic_pointer_cast<btSphereShape>(m_CollisionShape);
}

JSON::json SphereColliderComponent::getJSON() const
{
	JSON::json& j = PhysicsColliderComponent::getJSON();

	j["radius"] = m_Radius;

	return j;
}

void SphereColliderComponent::setRadius(float r)
{
	m_Radius = r;
	m_SphereShape->setUnscaledRadius(r);
}

void SphereColliderComponent::draw()
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
}
