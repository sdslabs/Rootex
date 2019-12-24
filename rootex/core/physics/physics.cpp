#include "physics.h"
#include "common/common.h"

void GamePhysics::initialize()
{
	m_CollisionConfiguration = new btDefaultCollisionConfiguration();
	m_Dispatcher = new btCollisionDispatcher(m_CollisionConfiguration);
	m_Broadphase = new btDbvtBroadphase();
	m_Solver = new btSequentialImpulseConstraintSolver;
	m_DynamicsWorld = new btDiscreteDynamicsWorld(m_Dispatcher, m_Broadphase, m_Solver, m_CollisionConfiguration);

	if (!m_CollisionConfiguration || !m_Dispatcher || !m_Broadphase || !m_Solver || !m_DynamicsWorld)
	{
		ERR("Initialization Failed !!");
		return;
	}
}

GamePhysics::~GamePhysics()
{
	//cleanup in the reverse order of creation/initialization

	//remove the rigidbodies from the dynamics world and delete them
	for(int i = m_DynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = m_DynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_DynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	delete (m_DynamicsWorld);
	delete (m_Solver);
	delete (m_Broadphase);
	delete (m_Dispatcher);
	delete (m_CollisionConfiguration);
}