#include "box_component.h"

BoxComponent::BoxComponent(Vector3 dimensions, const String& matName)
    : PhysicsComponent(matName, dimensions.x * dimensions.y * dimensions.z, Ref<btBoxShape>(new btBoxShape(vecTobtVector3(dimensions))))
    , m_Dimensions(dimensions)
{
	if (m_Mass > 0.f)
	{
		m_CollisionShape->calculateLocalInertia(m_Mass, m_LocalInertia);
	}
}

JSON::json BoxComponent::getJSON() const
{
	JSON::json j;

	j["dimensions"]["x"] = m_Dimensions.x;
	j["dimensions"]["y"] = m_Dimensions.y;
	j["dimensions"]["z"] = m_Dimensions.z;

	j["matName"] = m_MaterialName;

	return j;
}

Component* BoxComponent::Create(const JSON::json& boxComponentData)
{
	BoxComponent* component = new BoxComponent(
		{ boxComponentData["dimensions"]["x"], boxComponentData["dimensions"]["y"], boxComponentData["dimensions"]["z"] },
		boxComponentData["matName"]);
	return component;
}

Component* BoxComponent::CreateDefault()
{
	BoxComponent* component = new BoxComponent(
		{ 1.0f, 1.0f, 1.0f },
		"Air");
	return component;
}
