#include "sphere_component.h"

SphereColliderComponent::SphereColliderComponent(float rad, const String& matName)
    : PhysicsColliderComponent(matName, ((4.f / 3.f) * DirectX::XM_PI * rad * rad * rad), Ref<btSphereShape>(new btSphereShape(rad)))
    , m_Radius(rad)
{
	if (m_Mass > 0.f)
	{
		m_CollisionShape->calculateLocalInertia(m_Mass, m_LocalInertia);
	}
}

JSON::json SphereColliderComponent::getJSON() const
{
	JSON::json j;

	j["radius"] = m_Radius;
	j["matName"] = m_MaterialName;

	return j;
}

Component* SphereColliderComponent::Create(const JSON::json& sphereComponentData)
{
	SphereColliderComponent* component = new SphereColliderComponent(sphereComponentData["radius"], sphereComponentData["matName"]);
	return component;
}

Component* SphereColliderComponent::CreateDefault()
{
	SphereColliderComponent* component = new SphereColliderComponent(1.0f, "Air");
	return component;
}
