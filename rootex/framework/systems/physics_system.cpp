#include "physics_system.h"
#include "common/common.h"
#include "components/physics/physics_component.h"
#include "core/resource_loader.h"

PhysicsSystem* PhysicsSystem::GetSingleton()
{
	static PhysicsSystem singleton;
	return &singleton;
}

void PhysicsSystem::initialize()
{
	m_CollisionConfiguration.reset(new btDefaultCollisionConfiguration());
	m_Dispatcher.reset(new btCollisionDispatcher(m_CollisionConfiguration.get()));
	m_Broadphase.reset(new btDbvtBroadphase());
	m_Solver.reset(new btSequentialImpulseConstraintSolver);
	m_DynamicsWorld.reset(new btDiscreteDynamicsWorld(m_Dispatcher.get(), m_Broadphase.get(), m_Solver.get(), m_CollisionConfiguration.get()));

	LuaTextResourceFile* physicsMaterial = ResourceLoader::CreateLuaTextResourceFile("game/assets/config/physics.lua");
	LuaInterpreter::GetSingleton()->getLuaState().script(physicsMaterial->getString());
	physicsMaterialTable = LuaInterpreter::GetSingleton()->getLuaState()["PhysicsMaterial"];

	if (!m_CollisionConfiguration || !m_Dispatcher || !m_Broadphase || !m_Solver || !m_DynamicsWorld)
	{
		ERR("Initialization Failed!");
		return;
	}

	m_DynamicsWorld->setInternalTickCallback(internalTickCallback);
	m_DynamicsWorld->setWorldUserInfo(this);
}

PhysicsSystem::~PhysicsSystem()
{
	//cleanup in the reverse order of creation/initialization

	//remove the rigidbodies from the dynamics world and delete them
	for (int i = m_DynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = m_DynamicsWorld->getCollisionObjectArray()[i];
		m_DynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}
}

void PhysicsSystem::addRigidBody(btRigidBody* body)
{
	m_DynamicsWorld->addRigidBody(body);
}

sol::table PhysicsSystem::getPhysicsMaterial()
{
	return physicsMaterialTable;
}

// This function is called after bullet performs its internal update.
// To detect collisions between objects.
void PhysicsSystem::internalTickCallback(btDynamicsWorld* const world, btScalar const timeStep)
{
	PhysicsSystem* const physicsSystem = static_cast<PhysicsSystem*>(world->getWorldUserInfo());

	// look at all existing contacts
	btDispatcher* const dispatcher = world->getDispatcher();
	for (int manifoldIdx = 0; manifoldIdx < dispatcher->getNumManifolds(); ++manifoldIdx)
	{
		// get the "manifold",the set of data corresponding to a contact point
		//  between two physics objects
		btPersistentManifold const* const manifold = dispatcher->getManifoldByIndexInternal(manifoldIdx);

		// get the two bodies used in the manifold.
		btRigidBody const* const body0 = static_cast<btRigidBody const*>(manifold->getBody0());
		btRigidBody const* const body1 = static_cast<btRigidBody const*>(manifold->getBody1());

		WARN("Body collided");
	}
}

void PhysicsSystem::update(float deltaMilliseconds)
{
	m_DynamicsWorld->stepSimulation(deltaMilliseconds, 10);
}

void PhysicsSystem::syncVisibleScene()
{
	// Keep physics & graphics in sync

	// check all the existing bodies for changes.
	const Vector<Component*>& physicsComponents = s_Components[PhysicsComponent::s_ID];

	for (auto& physicsComponent : physicsComponents)
	{
		PhysicsComponent* const component = static_cast<PhysicsComponent*>(physicsComponent);
		if (component)
		{
			Ref<TransformComponent> ptc = component->m_TransformComponent;
			if (ptc)
			{
				if (ptc->getAbsoluteTransform() != component->m_MotionState.m_WorldToPositionTransform)
				{
					ptc->setTransform(component->m_MotionState.m_WorldToPositionTransform);
				}
			}
		}
	}
}
