#include "box_collider_component.h"
#include "framework/systems/physics_system.h"

Component* BoxColliderComponent::Create(const JSON::json& boxComponentData)
{
	BoxColliderComponent* component = new BoxColliderComponent(
		{ 
			boxComponentData["dimensions"]["x"], 
			boxComponentData["dimensions"]["y"], 
			boxComponentData["dimensions"]["z"] 
		},
		boxComponentData["matName"],
	    { 
			boxComponentData["gravity"]["x"],
	        boxComponentData["gravity"]["y"],
	        boxComponentData["gravity"]["z"]
		},
		boxComponentData["isMoveable"]);
	return component;
}

Component* BoxColliderComponent::CreateDefault()
{
	BoxColliderComponent* component = new BoxColliderComponent(
		{ 0.5f, 0.5f, 0.5f },
		"Air",
	    { 0.0f, 0.0f, 0.0f },
		false);
	return component;
}

BoxColliderComponent::BoxColliderComponent(const Vector3& dimensions, const String& matName, const Vector3& gravity, bool isMoveable)
    : PhysicsColliderComponent(matName, dimensions.x * dimensions.y * dimensions.z, gravity, isMoveable, Ref<btBoxShape>(new btBoxShape(vecTobtVector3(dimensions))))
    , m_Dimensions(dimensions)
{
	if (m_Mass > 0.0f)
	{
		m_CollisionShape->calculateLocalInertia(m_Mass, m_LocalInertia);
	}
	m_BoxShape = std::dynamic_pointer_cast<btBoxShape>(m_CollisionShape);
}

JSON::json BoxColliderComponent::getJSON() const
{
	JSON::json& j = PhysicsColliderComponent::getJSON();

	j["dimensions"]["x"] = m_Dimensions.x;
	j["dimensions"]["y"] = m_Dimensions.y;
	j["dimensions"]["z"] = m_Dimensions.z;

	j["matName"] = m_MaterialName;

	return j;
}

void BoxColliderComponent::setDimensions(const Vector3& dimensions)
{
	m_Dimensions = dimensions;
	m_BoxShape->setImplicitShapeDimensions(vecTobtVector3(dimensions));
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void BoxColliderComponent::draw()
{
	PhysicsColliderComponent::draw();

	if (ImGui::DragFloat3("##Dimensions", &m_Dimensions.x, 0.01f))
	{
		setDimensions(m_Dimensions);
	}
	ImGui::SameLine();
	if (ImGui::Button("Dimensions"))
	{
		m_Dimensions = { 0.5f, 0.5f, 0.5f };
		setDimensions(m_Dimensions);
	}
}
#endif // ROOTEX_EDITOR 
