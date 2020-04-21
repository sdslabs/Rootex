#pragma once

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

	btVector3 m_From;
	btVector3 m_To;

	PhysicsSystem()
	    = default;

public:
	static PhysicsSystem* GetSingleton();
	virtual ~PhysicsSystem();

	void addRigidBody(btRigidBody* body);
	sol::table getPhysicsMaterial();
	void castRays();

	/// Initialization and Maintenance of the Physics World
	void initialize();

	/// Callback from bullet for each physics time step.
	static void internalTickCallback(btDynamicsWorld* const world, btScalar const timeStep);

	void update(float deltaMilliseconds);
	// Keep physics & graphics in sync.
	// Check all the existing bodies for changes.
	virtual void syncVisibleScene();
};
