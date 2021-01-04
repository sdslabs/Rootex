#pragma once

#include "core/physics/debug_drawer.h"

#include "btBulletDynamicsCommon.h"
#include "entity.h"
#include "framework/system.h"

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

	/// Table of all the material types and their respective data.
	sol::table m_PhysicsMaterialTable;

	DebugDrawer m_DebugDrawer;

public:
	static PhysicsSystem* GetSingleton();
	PhysicsSystem();
	virtual ~PhysicsSystem() = default;

	void addRigidBody(btRigidBody* body);
	void removeRigidBody(btRigidBody* rigidBody);
	
	btCollisionWorld::AllHitsRayResultCallback reportAllRayHits(const btVector3& m_From, const btVector3& m_To);
	btCollisionWorld::ClosestRayResultCallback reportClosestRayHits(const btVector3& m_From, const btVector3& m_To);

	/// Initialization and Maintenance of the Physics World
	bool initialize(const JSON::json& systemData) override;

	/// Callback from bullet for each physics time step.
	static void InternalTickCallback(btDynamicsWorld* const world, btScalar const timeStep);

	void debugDraw();
	void debugDrawComponent(const btTransform& worldTransform, const btCollisionShape* shape, const btVector3& color);
	
	void update(float deltaMilliseconds) override;

	sol::table getPhysicsMaterial();
};
