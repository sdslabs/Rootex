#include "physics.h"
#include "common/common.h"

GamePhysics* GamePhysics::GetSingleton()
{
	static GamePhysics singleton;
	return &singleton;
}

void GamePhysics::initialize()
{
	m_CollisionConfiguration.reset(new btDefaultCollisionConfiguration());
	m_Dispatcher.reset(new btCollisionDispatcher(m_CollisionConfiguration.get()));
	m_Broadphase.reset(new btDbvtBroadphase());
	m_Solver.reset(new btSequentialImpulseConstraintSolver);
	m_DynamicsWorld.reset(new btDiscreteDynamicsWorld(m_Dispatcher.get(), m_Broadphase.get(), m_Solver.get(), m_CollisionConfiguration.get()));

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
}

void GamePhysics::addRigidBody(btRigidBody* body)
{
	m_DynamicsWorld->addRigidBody(body);
}

void GamePhysics::update(float deltaMilliseconds)
{
	m_DynamicsWorld.get()->stepSimulation(deltaMilliseconds, 10);
}
