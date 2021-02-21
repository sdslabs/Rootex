#include "static_mesh_collider_component.h"
#include "resource_loader.h"
#include "systems/physics_system.h"
#include "event_manager.h"

Component* StaticMeshColliderComponent::Create(const JSON::json& staticMeshComponentData)
{
	StaticMeshColliderComponent* component = new StaticMeshColliderComponent(
	    staticMeshComponentData.value("collisionModel", String("rootex/assets/cube.obj")),
	    staticMeshComponentData.value("material", PhysicsMaterial::Air),
	    staticMeshComponentData.value("collisionGroup", (int)CollisionMask::All),
	    staticMeshComponentData.value("collisionMask", (int)CollisionMask::All),
	    staticMeshComponentData.value("isGeneratesHitEvents", false));
	return component;
}

StaticMeshColliderComponent::StaticMeshColliderComponent(
	const String& collisionModelPath, 
	const PhysicsMaterial& material, 
	int collisionGroup, 
	int collisionMask, 
	bool generatesHitEvents)
    : PhysicsColliderComponent(
		material, 
		0.0f, 
		Vector3::Zero, 
		Vector3::Zero, 
		collisionGroup, 
		collisionMask, 
		false, 
		false, 
		generatesHitEvents,
        true,
		false,
		nullptr)
    , m_CollisionModel(ResourceLoader::CreateCollisionModelResourceFile(collisionModelPath))
{
	// m_MeshShape will be set during setup
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
