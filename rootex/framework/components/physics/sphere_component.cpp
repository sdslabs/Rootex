#include "sphere_component.h"

SphereComponent::SphereComponent(float rad, const String& matName)
    : PhysicsComponent(matName, ((4.f / 3.f) * DirectX::XM_PI * rad * rad * rad), Ref<btSphereShape>(new btSphereShape(rad)))
    , m_Radius(rad)
{
	if (m_Mass > 0.f)
	{
		m_CollisionShape->calculateLocalInertia(m_Mass, localInertia);
	}
}

JSON::json SphereComponent::getJSON() const
{
	JSON::json j;

	j["radius"] = m_Radius;
	j["matName"] = m_MaterialName;

	return j;
}

Component* SphereComponent::Create(const JSON::json& sphereComponentData)
{
	SphereComponent* component = new SphereComponent(sphereComponentData["radius"], sphereComponentData["matName"]);
	return component;
}

Component* SphereComponent::CreateDefault()
{
	SphereComponent* component = new SphereComponent(1.0f, "Air");
	return component;
}
