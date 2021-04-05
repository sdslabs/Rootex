#include "static_mesh_collider_component.h"
#include "resource_loader.h"
#include "systems/physics_system.h"
#include "event_manager.h"

Ptr<Component> StaticMeshColliderComponent::Create(const JSON::json& staticMeshComponentData)
{
	return std::make_unique<StaticMeshColliderComponent>(
	    ResourceLoader::CreateCollisionModelResourceFile(staticMeshComponentData.value("collisionModel", String("rootex/assets/cube.obj"))),
	    staticMeshComponentData.value("offset", Vector3(0.0f, 0.0f, 0.0f)),
	    staticMeshComponentData.value("material", PhysicsMaterial::Air),
	    staticMeshComponentData.value("collisionGroup", (int)CollisionMask::All),
	    staticMeshComponentData.value("collisionMask", (int)CollisionMask::All),
	    staticMeshComponentData.value("isGeneratesHitEvents", false));
}

StaticMeshColliderComponent::StaticMeshColliderComponent(
    Ref<CollisionModelResourceFile> file,
    const Vector3& offset,
    const PhysicsMaterial& material,
    int collisionGroup,
    int collisionMask,
    bool generatesHitEvents)
    : RigidBodyComponent(material, 0.0f, offset, Vector3::Zero, Vector3::Zero, collisionGroup, collisionMask, false, false, generatesHitEvents, true, false, nullptr)
    , m_CollisionModel(file)
{
	// m_MeshShape will be set during setup
}

bool StaticMeshColliderComponent::setupData()
{
	m_CollisionShape.reset(new btBvhTriangleMeshShape(m_CollisionModel->getCollisionMesh(), true));
	m_MeshShape = (btBvhTriangleMeshShape*)m_CollisionShape.get();
	return RigidBodyComponent::setupData();
}

void StaticMeshColliderComponent::createStaticMesh()
{
	detachCollisionObject();
	m_CollisionShape.reset(new btBvhTriangleMeshShape(m_CollisionModel->getCollisionMesh(), true));
	m_MeshShape = (btBvhTriangleMeshShape*)m_CollisionShape.get();
	m_Body->setCollisionShape(m_MeshShape);
	attachCollisionObject();
}

void StaticMeshColliderComponent::setCollisionModel(Ref<CollisionModelResourceFile> file)
{
	m_CollisionModel = file;
	createStaticMesh();
}

JSON::json StaticMeshColliderComponent::getJSON() const
{
	JSON::json j = RigidBodyComponent::getJSON();

	j["collisionModel"] = m_CollisionModel->getPath().generic_string();

	return j;
}

void StaticMeshColliderComponent::draw()
{
	RigidBodyComponent::draw();

	String filePath = m_CollisionModel->getPath().generic_string();
	ImGui::Text("%s", filePath.c_str());
	ImGui::SameLine();
	if (ImGui::Button("Collision Model"))
	{
		EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { m_CollisionModel->getPath().generic_string(), (int)m_CollisionModel->getType() });
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
