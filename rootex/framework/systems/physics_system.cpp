#include "physics_system.h"

#include "common/common.h"
#include "core/resource_loader.h"
#include "core/resource_files/lua_text_resource_file.h"

#include "components/physics/physics_collider_component.h"
#include "script/script.h"

#include "os/timer.h"
#include "render_system.h"

#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

PhysicsSystem* PhysicsSystem::GetSingleton()
{
	static PhysicsSystem singleton;
	return &singleton;
}

PhysicsSystem::PhysicsSystem()
    : System("PhysicsSystem", UpdateOrder::Update, true)
{
}

void PhysicsSystem::assignPhysicsMaterials()
{
	m_PhysicsMaterialTable[PhysicsMaterial::Air] = { 0.0f, 0.0f, 1.03e-3f };
	m_PhysicsMaterialTable[PhysicsMaterial::Water] = { 0.2f, 0.1f, 1.0f };
	m_PhysicsMaterialTable[PhysicsMaterial::Wood] = { 0.5f, 0.65f, 1.54f };
}

const char* PhysicsSystem::getMaterialNames()
{
	return "Air\0Water\0Wood\0";
}

bool PhysicsSystem::initialize(const JSON::json& systemData)
{
	m_CollisionConfiguration.reset(new btDefaultCollisionConfiguration());
	m_Dispatcher.reset(new btCollisionDispatcher(m_CollisionConfiguration.get()));
	m_Broadphase.reset(new btDbvtBroadphase());
	m_Solver.reset(new btSequentialImpulseConstraintSolver);
	m_DynamicsWorld.reset(new btDiscreteDynamicsWorld(m_Dispatcher.get(), m_Broadphase.get(), m_Solver.get(), m_CollisionConfiguration.get()));
	m_PhysicsMaterialTable.resize(PhysicsMaterial::End);
	assignPhysicsMaterials();

	if (!m_CollisionConfiguration || !m_Dispatcher || !m_Broadphase || !m_Solver || !m_DynamicsWorld || m_PhysicsMaterialTable.empty())
	{
		ERR("PhysicsSystem initialization failed");
		return false;
	}

	m_DynamicsWorld->setInternalTickCallback(InternalTickCallback);
	m_DynamicsWorld->setWorldUserInfo(this);
	m_DynamicsWorld->setDebugDrawer(&m_DebugDrawer);
	m_DynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

	return true;
}

void PhysicsSystem::addRigidBody(btRigidBody* body, int group, int mask)
{
	m_DynamicsWorld->addRigidBody(body, group, mask);
}

btCollisionWorld::AllHitsRayResultCallback PhysicsSystem::reportAllRayHits(const btVector3& from, const btVector3& to)
{
	m_DynamicsWorld->updateAabbs();
	m_DynamicsWorld->computeOverlappingPairs();
	btCollisionWorld::AllHitsRayResultCallback allResults(from, to);
	allResults.m_flags |= btTriangleRaycastCallback::kF_KeepUnflippedNormal;
	allResults.m_flags |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;
	m_DynamicsWorld->rayTest(from, to, allResults);
	return allResults;
}

btCollisionWorld::ClosestRayResultCallback PhysicsSystem::reportClosestRayHits(const btVector3& from, const btVector3& to)
{
	btCollisionWorld::ClosestRayResultCallback closestResults(from, to);
	closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
	m_DynamicsWorld->rayTest(from, to, closestResults);
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

		if (collider0->isGeneratesHitEvents())
		{
			collider0->getOwner()->call("onHit", { body0, body1 });
		}
		if (collider1->isGeneratesHitEvents())
		{
			collider1->getOwner()->call("onHit", { body1, body0 });
		}
	}
}

PhysicsSystem::~PhysicsSystem()
{
	int i;
	for (i = m_DynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = m_DynamicsWorld->getCollisionObjectArray()[i];
		m_DynamicsWorld->removeCollisionObject(obj);
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
	ZoneScoped;
	m_DynamicsWorld->stepSimulation(deltaMilliseconds * MS_TO_S, 10);
}

void PhysicsSystem::removeRigidBody(btRigidBody* rigidBody)
{
	m_DynamicsWorld->removeRigidBody(rigidBody);
}

void PhysicsSystem::removeCollisionObject(btCollisionObject* collisionObject)
{
	m_DynamicsWorld->removeCollisionObject(collisionObject);
}

const PhysicsMaterialData& PhysicsSystem::getMaterialData(PhysicsMaterial material)
{
	return m_PhysicsMaterialTable[material];
}
