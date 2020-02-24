#include "sphere_component.h"

SphereComponent::SphereComponent(float rad, const String& matName)
    : PhysicsComponent(matName, ((4.f / 3.f) * 3.14 * rad * rad * rad), Ref<btSphereShape>(new btSphereShape(rad)))
    , m_Radius(rad)
{
}

Component* SphereComponent::Create(const LuaVariable& sphereComponentData)
{
	SphereComponent* component = new SphereComponent(sphereComponentData["m_Radius"], sphereComponentData["matName"]);

	return component;
}
