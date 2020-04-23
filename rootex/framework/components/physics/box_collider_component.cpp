#include "box_collider_component.h"
#include "framework/systems/physics_system.h"

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

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void BoxColliderComponent::draw()
{
	ImGui::DragFloat3("##D", &m_Dimensions.x);
	ImGui::SameLine();
	if (ImGui::Button("Dimensions"))
	{
		m_Dimensions = { 1.0f, 1.0f, 1.0f };
	}

	if (ImGui::BeginCombo("Material", m_MaterialName.c_str()))
	{
		for (auto&& material : PhysicsSystem::GetSingleton()->getPhysicsMaterial())
		{
			if (ImGui::Selectable(material.first.as<String>().c_str()))
			{
				m_MaterialName.assign(material.first.as<String>().c_str());
			}
		}
		ImGui::EndCombo();
	}
}
#endif // ROOTEX_EDITOR 
