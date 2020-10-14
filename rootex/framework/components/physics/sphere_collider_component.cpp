#include "sphere_collider_component.h"
#include "framework/systems/physics_system.h"

Component* SphereColliderComponent::Create(const JSON::json& sphereComponentData)
{
	SphereColliderComponent* component = new SphereColliderComponent(
		sphereComponentData.value("radius", 1.0f), 
		sphereComponentData.value("matName", "Air"),
	    sphereComponentData.value("gravity", Vector3::Zero),
		sphereComponentData.value("isMoveable", false),
		sphereComponentData.value("isGeneratesHitEvents", false));
	return component;
}

Component* SphereColliderComponent::CreateDefault()
{
	SphereColliderComponent* component = new SphereColliderComponent(
		1.0f, 
		"Air", 
		{ 0.0f, 0.0f, 0.0f },
		false,
		false);
	return component;
}

SphereColliderComponent::SphereColliderComponent(float rad, const String& matName, const Vector3& gravity, bool isMoveable, bool generatesHitEvents)
    : PhysicsColliderComponent(matName, ((4.0f / 3.0f) * DirectX::XM_PI * rad * rad * rad), gravity, isMoveable, Ref<btSphereShape>(new btSphereShape(rad)), generatesHitEvents)
    , m_Radius(rad)
{
	if (m_Mass > 0.0f)
	{
		m_CollisionShape->calculateLocalInertia(m_Mass, m_LocalInertia);
	}
	m_SphereShape = std::dynamic_pointer_cast<btSphereShape>(m_CollisionShape);
}

JSON::json SphereColliderComponent::getJSON() const
{
	JSON::json& j = PhysicsColliderComponent::getJSON();

	j["radius"] = m_Radius;
	j["matName"] = m_MaterialName;

	return j;
}

void SphereColliderComponent::setRadius(float r)
{
	m_Radius = r;
	m_SphereShape->setUnscaledRadius(r);
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void SphereColliderComponent::draw()
{
	PhysicsColliderComponent::draw();

	if (ImGui::DragFloat("##Radius", &m_Radius, 0.01f))
	{
		setRadius(m_Radius);
	}
	ImGui::SameLine();
	if (ImGui::Button("Radius"))
	{
		m_Radius = 1.0f;
	}
}
#endif // ROOTEX_EDITOR
