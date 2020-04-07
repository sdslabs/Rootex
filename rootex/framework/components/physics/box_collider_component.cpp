#include "box_collider_component.h"

BoxColliderComponent::BoxColliderComponent(const Vector3 dimensions, const String& matName)
    : PhysicsColliderComponent(matName, dimensions.x * dimensions.y * dimensions.z, Ref<btBoxShape>(new btBoxShape(vecTobtVector3(dimensions))))
    , m_Dimensions(dimensions)
{
	if (m_Mass > 0.f)
	{
		m_CollisionShape->calculateLocalInertia(m_Mass, m_LocalInertia);
	}
}

JSON::json BoxColliderComponent::getJSON() const
{
	JSON::json j;

	j["dimensions"]["x"] = m_Dimensions.x;
	j["dimensions"]["y"] = m_Dimensions.y;
	j["dimensions"]["z"] = m_Dimensions.z;

	j["matName"] = m_MaterialName;

	return j;
}

Component* BoxColliderComponent::Create(const JSON::json& boxComponentData)
{
	BoxColliderComponent* component = new BoxColliderComponent(
		{ boxComponentData["dimensions"]["x"], boxComponentData["dimensions"]["y"], boxComponentData["dimensions"]["z"] },
		boxComponentData["matName"]);
	return component;
}

Component* BoxColliderComponent::CreateDefault()
{
	BoxColliderComponent* component = new BoxColliderComponent(
		{ 1.0f, 1.0f, 1.0f },
		"Air");
	return component;
}
