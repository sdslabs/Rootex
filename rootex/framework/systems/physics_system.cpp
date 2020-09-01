#include "physics_system.h"

#include "core/resource_loader.h"

#include "common/common.h"

#include "components/physics/physics_collider_component.h"
#include "components/script_component.h"

#include "os/timer.h"
#include "render_system.h"

#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

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
	m_PhysicsMaterialTable = LuaInterpreter::GetSingleton()->getLuaState()["PhysicsMaterial"];

	if (!m_CollisionConfiguration || !m_Dispatcher || !m_Broadphase || !m_Solver || !m_DynamicsWorld)
	{
		ERR("Initialization Failed!");
		return;
	}

	m_DynamicsWorld->setInternalTickCallback(InternalTickCallback);
	m_DynamicsWorld->setWorldUserInfo(this);
	m_DynamicsWorld->setDebugDrawer(&m_DebugDrawer);
	m_DynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
}

PhysicsSystem::~PhysicsSystem()
{
	//cleanup in the reverse order of creation/initialization
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
	return m_PhysicsMaterialTable;
}

btCollisionWorld::AllHitsRayResultCallback PhysicsSystem::reportAllRayHits(const btVector3& m_From, const btVector3& m_To)
{
	m_DynamicsWorld->updateAabbs();
	m_DynamicsWorld->computeOverlappingPairs();

	btCollisionWorld::AllHitsRayResultCallback allResults(m_From, m_To);
	allResults.m_flags |= btTriangleRaycastCallback::kF_KeepUnflippedNormal;
	allResults.m_flags |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;

	m_DynamicsWorld->rayTest(m_From, m_To, allResults);
	return allResults;
}

btCollisionWorld::ClosestRayResultCallback PhysicsSystem::reportClosestRayHits(const btVector3& m_From, const btVector3& m_To)
{
	btCollisionWorld::ClosestRayResultCallback closestResults(m_From, m_To);
	closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

	m_DynamicsWorld->rayTest(m_From, m_To, closestResults);

	return closestResults;
}

// This function is called after bullet performs its internal update.
// To detect collisions between objects.
void PhysicsSystem::InternalTickCallback(btDynamicsWorld* const world, btScalar const timeStep)
{
	PhysicsSystem* const physicsSystem = static_cast<PhysicsSystem*>(world->getWorldUserInfo());

	// look at all existing contacts
	btDispatcher* const dispatcher = world->getDispatcher();
	for (int manifoldIdx = 0; manifoldIdx < dispatcher->getNumManifolds(); ++manifoldIdx)
	{
		// get the "manifold", the set of data corresponding to a contact point
		// between two physics objects
		btPersistentManifold* manifold = dispatcher->getManifoldByIndexInternal(manifoldIdx);

		// get the two bodies used in the manifold.
		btRigidBody const* const body0 = static_cast<btRigidBody const*>(manifold->getBody0());
		btRigidBody const* const body1 = static_cast<btRigidBody const*>(manifold->getBody1());

		PhysicsColliderComponent* collider0 = (PhysicsColliderComponent*)body0->getUserPointer();
		PhysicsColliderComponent* collider1 = (PhysicsColliderComponent*)body1->getUserPointer();

		if (collider0->getIsGeneratesHitEvents())
		{
			if (collider0->getScriptComponent())
			{
				collider0->getScriptComponent()->onHit(manifold, collider1);
			}
		}
		if (collider1->getIsGeneratesHitEvents())
		{
			if (collider1->getScriptComponent())
			{
				collider1->getScriptComponent()->onHit(manifold, collider0);
			}
		}
	}
}

void PhysicsSystem::debugDraw()
{
	RenderSystem::GetSingleton()->getRenderer()->bind(m_DebugDrawer.getMaterial());

	RenderSystem::GetSingleton()->enableLineRenderMode();
	for (auto& component : s_Components[PhysicsColliderComponent::s_ID])
	{
		PhysicsColliderComponent* p = (PhysicsColliderComponent*)component;
		p->render();
	}
	RenderSystem::GetSingleton()->resetRenderMode();
}

void PhysicsSystem::debugDrawComponent(const btTransform& worldTransform, const btCollisionShape* shape, const btVector3& color)
{
	m_DynamicsWorld->debugDrawObject(worldTransform, shape, color);
}

void PhysicsSystem::update(float deltaMilliseconds)
{
	m_DynamicsWorld->stepSimulation(deltaMilliseconds * MS_TO_S, 10);
}

void PhysicsSystem::removeRigidBody(btRigidBody* rigidBody)
{
	m_DynamicsWorld->removeRigidBody(rigidBody);
}
