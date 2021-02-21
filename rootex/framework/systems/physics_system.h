#pragma once

#include "core/physics/debug_drawer.h"

#include "btBulletDynamicsCommon.h"
#include "entity.h"
#include "framework/system.h"

enum PhysicsMaterial
{
	Air = 0,
	Water = 1,
	Wood = 2,
	End
};

struct PhysicsMaterialData
{
	float restitution = 1.0f;
	float friction = 1.0f;
	float specificGravity = 1.0f;
};

class PhysicsSystem : public System
{
	/// Interface for several dynamics implementations, basic, discrete, parallel, and continuous etc.
	Ptr<btDynamicsWorld> m_DynamicsWorld;

	/// An interface to detect aabb-overlapping object pairs.
	Ptr<btBroadphaseInterface> m_Broadphase;

	/// btCollisionDispatcher supports algorithms that handle ConvexConvex and ConvexConcave collision pairs.
	/// Time of Impact, Closest Points and Penetration Depth.
	Ptr<btCollisionDispatcher> m_Dispatcher;

	/// Provides solver interface.
	Ptr<btConstraintSolver> m_Solver;

	/// Allows to configure Bullet collision detection.
	Ptr<btDefaultCollisionConfiguration> m_CollisionConfiguration;

	/// Draws all debug data in the physics world.
	DebugDrawer m_DebugDrawer;

	Vector<PhysicsMaterialData> m_PhysicsMaterialTable;
	String m_PhysicsMaterialNames;

	PhysicsSystem();

	void assignPhysicsMaterials();

public:
	static PhysicsSystem* GetSingleton();

	/// Callback from bullet for each physics time step.
	static void InternalTickCallback(btDynamicsWorld* const world, btScalar const timeStep);

	virtual ~PhysicsSystem();

	/// Initialization and Maintenance of the Physics World
	bool initialize(const JSON::json& systemData) override;

	void addRigidBody(btRigidBody* body, int group, int mask);
	void removeRigidBody(btRigidBody* rigidBody);
	void removeCollisionObject(btCollisionObject* collisionObject);

	const PhysicsMaterialData& getMaterialData(PhysicsMaterial material);
	const char* getMaterialNames();

	btCollisionWorld::AllHitsRayResultCallback reportAllRayHits(const btVector3& m_From, const btVector3& m_To);
	btCollisionWorld::ClosestRayResultCallback reportClosestRayHits(const btVector3& m_From, const btVector3& m_To);

	void debugDrawComponent(const btTransform& worldTransform, const btCollisionShape* shape, const btVector3& color);

	void update(float deltaMilliseconds) override;
};
