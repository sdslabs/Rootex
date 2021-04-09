#include "box_collider_component.h"
#include "framework/systems/physics_system.h"

BoxColliderComponent::BoxColliderComponent(Entity& owner, const JSON::json& data)
    : RigidBodyComponent(
        owner,
        data.value("material", PhysicsMaterial::Air),
        data.value("dimensions", Vector3 { 0.5f, 0.5f, 0.5f }).x * data.value("dimensions", Vector3 { 0.5f, 0.5f, 0.5f }).y * data.value("dimensions", Vector3 { 0.5f, 0.5f, 0.5f }).z,
        data.value("offset", Vector3(0.0f, 0.0f, 0.0f)),
        data.value("gravity", Vector3(0.0f, -9.8f, 0.0f)),
        data.value("angularFactor", Vector3::One),
        data.value("collisionGroup", (int)CollisionMask::All),
        data.value("collisionMask", (int)CollisionMask::All),
        data.value("isMoveable", false),
        data.value("isKinematic", false),
        data.value("isGeneratesHitEvents", false),
        data.value("isSleepable", true),
        data.value("isCCD", false),
        Ref<btBoxShape>(new btBoxShape(VecTobtVector3(data.value("dimensions", Vector3 { 0.5f, 0.5f, 0.5f })))))
    , m_Dimensions(data.value("dimensions", Vector3 { 0.5f, 0.5f, 0.5f }))
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
