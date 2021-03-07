#pragma once

#include "physics_collider_component.h"
#include "resource_files/collision_model_resource_file.h"

#include "btBulletCollisionCommon.h"

class StaticMeshColliderComponent : public PhysicsColliderComponent
{
	static Component* Create(const JSON::json& staticMeshComponentData);

	btBvhTriangleMeshShape* m_MeshShape;
	CollisionModelResourceFile* m_CollisionModel;

	friend class ECSFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::StaticMeshColliderComponent;

	StaticMeshColliderComponent(
	    const String& collisionModelPath,
	    const PhysicsMaterial& material,
	    int collisionGroup,
	    int collisionMask,
	    bool generatesHitEvents);

	bool setupData() override;
	void setCollisionModel(CollisionModelResourceFile* file);

	virtual const char* getName() const override { return "StaticMeshColliderComponent"; };
	virtual JSON::json getJSON() const override;
	virtual ComponentID getComponentID() const override { return s_ID; }

	void draw() override;
};
