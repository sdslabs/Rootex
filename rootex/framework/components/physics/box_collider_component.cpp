#include "box_collider_component.h"
#include "framework/systems/physics_system.h"

Ptr<Component> BoxColliderComponent::Create(const JSON::json& boxComponentData)
{
	return std::make_unique<BoxColliderComponent>(
	    boxComponentData.value("dimensions", Vector3 { 0.5f, 0.5f, 0.5f }),
	    boxComponentData.value("offset", Vector3(0.0f, 0.0f, 0.0f)),
	    boxComponentData.value("material", PhysicsMaterial::Air),
	    boxComponentData.value("angularFactor", Vector3::One),
	    boxComponentData.value("gravity", Vector3(0.0f, -9.8f, 0.0f)),
	    boxComponentData.value("collisionGroup", (int)CollisionMask::All),
	    boxComponentData.value("collisionMask", (int)CollisionMask::All),
	    boxComponentData.value("isMoveable", false),
	    boxComponentData.value("isKinematic", false),
	    boxComponentData.value("isGeneratesHitEvents", false),
	    boxComponentData.value("isSleepable", true),
	    boxComponentData.value("isCCD", false));
}

BoxColliderComponent::BoxColliderComponent(
    const Vector3& dimensions,
    const Vector3& offset,
    const PhysicsMaterial& material,
    const Vector3& angularFactor,
    const Vector3& gravity,
    int collisionGroup,
    int collisionMask,
    bool isMoveable,
    bool isKinematic,
    bool generatesHitEvents,
    bool isSleepable,
    bool isCCD)
    : RigidBodyComponent(
        material,
        dimensions.x * dimensions.y * dimensions.z,
        offset,
        gravity,
        angularFactor,
        collisionGroup,
        collisionMask,
        isMoveable,
        isKinematic,
        generatesHitEvents,
        isSleepable,
        isCCD,
        Ref<btBoxShape>(new btBoxShape(VecTobtVector3(dimensions))))
    , m_Dimensions(dimensions)
{
	m_BoxShape = (btBoxShape*)m_CollisionShape.get();
}

JSON::json BoxColliderComponent::getJSON() const
{
	JSON::json& j = RigidBodyComponent::getJSON();

	j["dimensions"] = m_Dimensions;

	return j;
}

void BoxColliderComponent::setDimensions(const Vector3& dimensions)
{
	detachCollisionObject();
	m_Dimensions = dimensions;
	m_CollisionShape.reset(new btBoxShape(VecTobtVector3(dimensions)));
	m_BoxShape = (btBoxShape*)m_CollisionShape.get();
	m_Body->setCollisionShape(m_BoxShape);
	attachCollisionObject();
}

void BoxColliderComponent::draw()
{
	RigidBodyComponent::draw();

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
