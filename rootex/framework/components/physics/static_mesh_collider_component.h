#pragma once

#include "rigid_body_component.h"
#include "resource_files/collision_model_resource_file.h"

#include "btBulletCollisionCommon.h"

/// Collision component with a 3D model as its shape.
class StaticMeshColliderComponent : public RigidBodyComponent
{
	COMPONENT(StaticMeshColliderComponent, Category::Physics);

	btBvhTriangleMeshShape* m_MeshShape;
	Ref<CollisionModelResourceFile> m_CollisionModel;

	void createStaticMesh();

public:
	StaticMeshColliderComponent(Entity& owner, const JSON::json& data);
	~StaticMeshColliderComponent() = default;

	bool setupData() override;
	void setCollisionModel(Ref<CollisionModelResourceFile> file);

	JSON::json getJSON() const override;
	void draw() override;
};

DECLARE_COMPONENT(StaticMeshColliderComponent);
