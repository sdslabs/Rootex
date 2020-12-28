#include "static_mesh_collider_component.h"
#include "resource_loader.h"
#include "systems/physics_system.h"

Component* StaticMeshColliderComponent::Create(const JSON::json& staticMeshComponentData)
{
	StaticMeshColliderComponent* component = new StaticMeshColliderComponent(
	    staticMeshComponentData.value("collisionModel", String("rootex/assets/cube.obj")),
	    staticMeshComponentData.value("material", PhysicsMaterial::Air),
	    staticMeshComponentData.value("angularFactor", Vector3::One),
	    staticMeshComponentData.value("gravity", Vector3(0.0f, -9.8f, 0.0f)),
	    staticMeshComponentData.value("isMoveable", false),
	    staticMeshComponentData.value("isKinematic", false),
	    staticMeshComponentData.value("isGeneratesHitEvents", false));
	return component;
}

StaticMeshColliderComponent::StaticMeshColliderComponent(const String& collisionModelPath, const PhysicsMaterial& material, const Vector3& angularFactor, const Vector3& gravity, bool isMoveable, bool isKinematic, bool generatesHitEvents)
    : PhysicsColliderComponent(material, 0.0f, gravity, angularFactor, isMoveable, isKinematic, generatesHitEvents, nullptr)
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
#include "ImGuiFileDialog.h"
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
		igfd::ImGuiFileDialog::Instance()->OpenModal("ChooseCollisionModelComponentModel", "Choose Collision Model File", SupportedFiles.at(ResourceFile::Type::CollisionModel), "game/assets/");
	}

	if (igfd::ImGuiFileDialog::Instance()->FileDialog("ChooseCollisionModelComponentModel"))
	{
		if (igfd::ImGuiFileDialog::Instance()->IsOk)
		{
			FilePath filePath = OS::GetRootRelativePath(igfd::ImGuiFileDialog::Instance()->GetFilePathName());
			setCollisionModel(ResourceLoader::CreateCollisionModelResourceFile(filePath.generic_string()));
		}
		igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseCollisionModelComponentModel");
	}
}
#endif // ROOTEX_EDITOR
