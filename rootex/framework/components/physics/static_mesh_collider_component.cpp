#include "static_mesh_collider_component.h"
#include "resource_loader.h"
#include "systems/physics_system.h"
#include "event_manager.h"

DEFINE_COMPONENT(StaticMeshColliderComponent);

StaticMeshColliderComponent::StaticMeshColliderComponent(Entity& owner, const JSON::json& data)
    : RigidBodyComponent(
        owner,
        data.value("material", PhysicsMaterial::Air),
        0.0f,
        data.value("offset", Vector3(0.0f, 0.0f, 0.0f)),
        Vector3::Zero,
        Vector3::Zero,
        data.value("collisionGroup", (int)CollisionMask::All),
        data.value("collisionMask", (int)CollisionMask::All),
        false,
        false,
        data.value("isGeneratesHitEvents", false),
        true,
        false,
        nullptr)
    , m_CollisionModel(ResourceLoader::CreateCollisionModelResourceFile(data.value("collisionModel", String("rootex/assets/cube.obj"))))
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

	ImGui::BeginGroup();
	String filePath = m_CollisionModel->getPath().generic_string();
	ImGui::Text("%s", filePath.c_str());
	ImGui::SameLine();
	if (ImGui::Button("Collision Model"))
	{
		EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { m_CollisionModel->getPath().generic_string(), (int)m_CollisionModel->getType() });
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_FOLDER_OPEN "##Collision Model File"))
	{
		if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::CollisionModel), "game/assets/"))
		{
			setCollisionModel(ResourceLoader::CreateCollisionModelResourceFile(*result));
		}
	}
	ImGui::EndGroup();
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("COLLISION_MODEL_PAYLOAD"))
		{
			const char* path = (const char*)payload->Data;
			setCollisionModel(ResourceLoader::CreateCollisionModelResourceFile(path));
		}
	}
}
