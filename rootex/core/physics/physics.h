#pragma once

#include"btBulletDynamicsCommon.h"
#include"entity.h"
#include"framework/system.h"

#include <map>

class GamePhysics : public System
{
	//The btDynamicsWorld is the interface class for several dynamics implementation, basic, discrete, parallel, and continuous etc.
	Ptr<btDynamicsWorld> m_DynamicsWorld;

	//The btBroadphaseInterface class provides an interface to detect aabb-overlapping object pairs.
	Ptr<btBroadphaseInterface> m_Broadphase;

	//btCollisionDispatcher supports algorithms that handle ConvexConvex and ConvexConcave collision pairs.
	//Time of Impact, Closest Points and Penetration Depth.
	Ptr<btCollisionDispatcher> m_Dispatcher;

	// btConstraintSolver provides solver interface
	Ptr<btConstraintSolver> m_Solver;

	//btCollisionConfiguration allows to configure Bullet collision detection
	Ptr<btDefaultCollisionConfiguration> m_CollisionConfiguration;


public:
	GamePhysics() {};
	virtual ~GamePhysics();
	
	// Initialization and Maintenance of the Physics World
	void initialize();

	void update(float deltaMilliseconds) override;
};
