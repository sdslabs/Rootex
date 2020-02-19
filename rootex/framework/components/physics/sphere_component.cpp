#include "sphere_component.h"

SphereComponent::SphereComponent(float rad, const String& matName)
    : PhysicsComponent(matName, ((4.f / 3.f) * 3.14 * rad * rad * rad))
    , m_Radius(rad)
{
	collisionShape.reset(new btSphereShape(rad));
}

Component* SphereComponent::Create(const LuaVariable& sphereComponentData)
{
	SphereComponent* component = new SphereComponent(0.0f, "air");

	return component;
}
