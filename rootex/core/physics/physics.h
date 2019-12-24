#pragma once

#include"btBulletDynamicsCommon.h"

class GamePhysics
{
	//The btDynamicsWorld is the interface class for several dynamics implementation, basic, discrete, parallel, and continuous etc.
	btDynamicsWorld* m_DynamicsWorld;

	//The btBroadphaseInterface class provides an interface to detect aabb-overlapping object pairs.
	btBroadphaseInterface* m_Broadphase;

	//btCollisionDispatcher supports algorithms that handle ConvexConvex and ConvexConcave collision pairs.
	//Time of Impact, Closest Points and Penetration Depth.
	btCollisionDispatcher* m_Dispatcher;

	// btConstraintSolver provides solver interface
	btConstraintSolver* m_Solver;

	//btCollisionConfiguration allows to configure Bullet collision detection
	btDefaultCollisionConfiguration* m_CollisionConfiguration;

public:
	GAmePhysics() {};
	virtual ~GamePhysics();
	
	// Initialization and Maintenance of the Physics World
	void initialize();
};