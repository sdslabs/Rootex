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

	GamePhysics() = default;

public:
	static GamePhysics* GetSingleton();
	virtual ~GamePhysics();

	void addRigidBody(btRigidBody* body);
	
	// Initialization and Maintenance of the Physics World
	void initialize();

	static btTransform Mat4x4_to_btTransform(Matrix const& mat)
	{
		// convert from Mat4x4 to btTransform
		btMatrix3x3 bulletRotation;
		btVector3 bulletPosition;

		// copy rotation matrix
		for (int row = 0; row < 3; ++row)
			for (int column = 0; column < 3; ++column)
				bulletRotation[row][column] = mat.m[column][row]; // note the reversed indexing (row/column vs. column/row)
				    //  this is because Mat4x4s are row-major matrices and
				    //  btMatrix3x3 are column-major.  This reversed indexing
				    //  implicitly transposes (flips along the diagonal)
				    //  the matrix when it is copied.

		// copy position
		for (int column = 0; column < 3; ++column)
			bulletPosition[column] = mat.m[3][column];

		return btTransform(bulletRotation, bulletPosition);
	}

	void update(float deltaMilliseconds) override;
};
