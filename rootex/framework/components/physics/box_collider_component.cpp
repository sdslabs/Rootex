#include "box_collider_component.h"
#include "framework/systems/physics_system.h"

Component* BoxColliderComponent::Create(const JSON::json& boxComponentData)
{
	BoxColliderComponent* component = new BoxColliderComponent(
	    boxComponentData.value("dimensions", Vector3::Zero),
		boxComponentData.value("material", PhysicsMaterial::Air),
	    boxComponentData.value("gravity", Vector3::Zero),
	    boxComponentData.value("isMoveable", false),
	    boxComponentData.value("isKinematic", false),
		boxComponentData.value("isGeneratesHitEvents", false));
	return component;
}

BoxColliderComponent::BoxColliderComponent(const Vector3& dimensions, const PhysicsMaterial& material, const Vector3& gravity, bool isMoveable, bool isKinematic, bool generatesHitEvents)
    : PhysicsColliderComponent(material, dimensions.x * dimensions.y * dimensions.z, gravity, isMoveable, isKinematic, generatesHitEvents, Ref<btBoxShape>(new btBoxShape(VecTobtVector3(dimensions))))
    , m_Dimensions(dimensions)
{
	m_BoxShape = std::dynamic_pointer_cast<btBoxShape>(m_CollisionShape);
}

JSON::json BoxColliderComponent::getJSON() const
{
	JSON::json& j = PhysicsColliderComponent::getJSON();

	j["dimensions"] = m_Dimensions;

	return j;
}

void BoxColliderComponent::setDimensions(const Vector3& dimensions)
{
	m_Dimensions = dimensions;
	m_BoxShape->setImplicitShapeDimensions(VecTobtVector3(dimensions));
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
