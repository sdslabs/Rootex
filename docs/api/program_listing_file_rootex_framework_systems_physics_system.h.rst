
.. _program_listing_file_rootex_framework_systems_physics_system.h:

Program Listing for File physics_system.h
=========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_systems_physics_system.h>` (``rootex\framework\systems\physics_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "core/physics/debug_drawer.h"
   #include "entity.h"
   #include "framework/system.h"
   
   #include "btBulletDynamicsCommon.h"
   #include "BulletCollision/CollisionDispatch/btGhostObject.h"
   
   enum PhysicsMaterial
   {
       Air = 0,
       Water = 1,
       Wood = 2,
       End
   };
   
   struct PhysicsMaterialData
   {
       float restitution = 1.0f;
       float friction = 1.0f;
       float specificGravity = 1.0f;
   };
   
   class PhysicsSystem : public System
   {
       Ptr<btDynamicsWorld> m_DynamicsWorld;
   
       Ptr<btBroadphaseInterface> m_Broadphase;
   
       Ptr<btCollisionDispatcher> m_Dispatcher;
   
       Ptr<btConstraintSolver> m_Solver;
   
       Ptr<btDefaultCollisionConfiguration> m_CollisionConfiguration;
   
       Ptr<btGhostPairCallback> m_GhostPairCallback;
   
       DebugDrawer m_DebugDrawer;
   
       Vector<PhysicsMaterialData> m_PhysicsMaterialTable;
       String m_PhysicsMaterialNames;
   
       PhysicsSystem();
   
       void assignPhysicsMaterials();
   
   public:
       static PhysicsSystem* GetSingleton();
   
       static void InternalTickCallback(btDynamicsWorld* const world, btScalar const timeStep);
   
       virtual ~PhysicsSystem();
   
       bool initialize(const JSON::json& systemData) override;
   
       void addRigidBody(btRigidBody* body, int group, int mask);
       void removeRigidBody(btRigidBody* rigidBody);
   
       void addCollisionObject(btCollisionObject* body, int group, int mask);
       void removeCollisionObject(btCollisionObject* collisionObject);
   
       const PhysicsMaterialData& getMaterialData(PhysicsMaterial material);
       const char* getMaterialNames();
   
       btCollisionWorld::AllHitsRayResultCallback reportAllRayHits(const btVector3& m_From, const btVector3& m_To);
       btCollisionWorld::ClosestRayResultCallback reportClosestRayHits(const btVector3& m_From, const btVector3& m_To);
   
       void debugDrawComponent(const btTransform& worldTransform, const btCollisionShape* shape, const btVector3& color);
   
       void update(float deltaMilliseconds) override;
   };
