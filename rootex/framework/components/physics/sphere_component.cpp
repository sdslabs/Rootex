#include "sphere_component.h"

SphereComponent::SphereComponent(float rad, const String& matName)
    : PhysicsComponent(matName, ((4.0f / 3.0f) * DirectX::XM_PI * rad * rad * rad))
    , m_Radius(rad)
{
	collisionShape.reset(new btSphereShape(rad));
	if (m_Mass > 0.f)
	{
		collisionShape->calculateLocalInertia(m_Mass, localInertia);
	}
}

Component* SphereComponent::Create(const LuaVariable& sphereComponentData)
{
	SphereComponent* component = new SphereComponent(sphereComponentData["m_Radius"], "air");
	return component;
}
