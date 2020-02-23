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

Component* SphereComponent::Create(const LuaVariable& sphereComponentData)
{
	SphereComponent* component = new SphereComponent(sphereComponentData["m_Radius"], sphereComponentData["matName"]);
	return component;
}
