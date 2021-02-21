#include "static_mesh_collider_component.h"
#include "resource_loader.h"
#include "systems/physics_system.h"
#include "event_manager.h"

Component* StaticMeshColliderComponent::Create(const JSON::json& staticMeshComponentData)
{
	StaticMeshColliderComponent* component = new StaticMeshColliderComponent(
	    staticMeshComponentData.value("collisionModel", String("rootex/assets/cube.obj")),
	    staticMeshComponentData.value("material", PhysicsMaterial::Air),
	    staticMeshComponentData.value("angularFactor", Vector3::One),
	    staticMeshComponentData.value("gravity", Vector3(0.0f, -9.8f, 0.0f)),
	    staticMeshComponentData.value("collisionGroup", (int)CollisionMask::All),
	    staticMeshComponentData.value("collisionMask", (int)CollisionMask::All),
	    staticMeshComponentData.value("isMoveable", false),
	    staticMeshComponentData.value("isKinematic", false),
	    staticMeshComponentData.value("isGeneratesHitEvents", false));
	return component;
}

StaticMeshColliderComponent::StaticMeshColliderComponent(const String& collisionModelPath, const PhysicsMaterial& material, const Vector3& angularFactor, const Vector3& gravity, int collisionGroup, int collisionMask, bool isMoveable, bool isKinematic, bool generatesHitEvents)
    : PhysicsColliderComponent(material, 0.0f, gravity, angularFactor, collisionGroup, collisionMask, isMoveable, isKinematic, generatesHitEvents, nullptr)
    , m_CollisionModel(ResourceLoader::CreateCollisionModelResourceFile(collisionModelPath))
{
}

bool StaticMeshColliderComponent::setupData()
{
	m_MeshShape.reset(new btBvhTriangleMeshShape(m_CollisionModel->getCollisionMesh(), true));
	m_CollisionShape = m_MeshShape;
	return PhysicsColliderComponent::setupData();
}

void StaticMeshColliderComponent::setCollisionModel(CollisionModelResourceFile* file)
{
	m_CollisionModel = file;
	setupData();
}

JSON::json StaticMeshColliderComponent::getJSON() const
{
	JSON::json j = PhysicsColliderComponent::getJSON();

	j["collisionModel"] = m_CollisionModel->getPath().generic_string();

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui_helpers.h"
void StaticMeshColliderComponent::draw()
{
	PhysicsColliderComponent::draw();

	String filePath = m_CollisionModel->getPath().generic_string();
	ImGui::Text("%s", filePath.c_str());
	ImGui::SameLine();
	if (ImGui::Button("Collision Model"))
	{
		EventManager::GetSingleton()->call("OpenCollisionModel", "EditorOpenFile", m_CollisionModel->getPath().string());
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_PENCIL_SQUARE_O "##Collision Model File"))
	{
		if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::CollisionModel), "game/assets/"))
		{
			setCollisionModel(ResourceLoader::CreateCollisionModelResourceFile(*result));
		}
	}
}
#endif // ROOTEX_EDITOR
