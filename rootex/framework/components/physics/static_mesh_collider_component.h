#pragma once

#include "rigid_body_component.h"
#include "resource_files/collision_model_resource_file.h"

#include "btBulletCollisionCommon.h"

class StaticMeshColliderComponent : public RigidBodyComponent
{
	DEFINE_COMPONENT(StaticMeshColliderComponent);

	btBvhTriangleMeshShape* m_MeshShape;
	Ref<CollisionModelResourceFile> m_CollisionModel;

	void createStaticMesh();

public:
	StaticMeshColliderComponent(
	    Ref<CollisionModelResourceFile> file,
	    const Vector3& offset,
	    const PhysicsMaterial& material,
	    int collisionGroup,
	    int collisionMask,
	    bool generatesHitEvents);
	~StaticMeshColliderComponent() = default;

	bool setupData() override;
	void setCollisionModel(Ref<CollisionModelResourceFile> file);

	JSON::json getJSON() const override;
	void draw() override;
};
