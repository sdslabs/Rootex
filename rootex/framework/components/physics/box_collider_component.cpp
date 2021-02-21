#include "box_collider_component.h"
#include "framework/systems/physics_system.h"

Component* BoxColliderComponent::Create(const JSON::json& boxComponentData)
{
	BoxColliderComponent* component = new BoxColliderComponent(
	    boxComponentData.value("dimensions", Vector3::Zero),
	    boxComponentData.value("material", PhysicsMaterial::Air),
	    boxComponentData.value("angularFactor", Vector3::One),
	    boxComponentData.value("gravity", Vector3(0.0f, -9.8f, 0.0f)),
	    boxComponentData.value("collisionGroup", (int)CollisionMask::All),
	    boxComponentData.value("collisionMask", (int)CollisionMask::All),
	    boxComponentData.value("isMoveable", false),
	    boxComponentData.value("isKinematic", false),
	    boxComponentData.value("isGeneratesHitEvents", false),
	    boxComponentData.value("isSleepable", true));
	return component;
}

BoxColliderComponent::BoxColliderComponent(
	const Vector3& dimensions, 
	const PhysicsMaterial& material, 
	const Vector3& angularFactor, 
	const Vector3& gravity, 
	int collisionGroup, 
	int collisionMask, 
	bool isMoveable, 
	bool isKinematic, 
	bool generatesHitEvents, 
	bool isSleepable)
    : PhysicsColliderComponent(
		material, 
		dimensions.x * dimensions.y * dimensions.z, 
		gravity, 
		angularFactor, 
		collisionGroup, 
		collisionMask, 
		isMoveable, 
		isKinematic, 
		generatesHitEvents, 
		isSleepable,
		Ref<btBoxShape>(new btBoxShape(VecTobtVector3(dimensions))))
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
	m_BoxShape.reset(new btBoxShape(VecTobtVector3(dimensions)));
	m_CollisionShape = m_BoxShape;
	setupData();
}

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
