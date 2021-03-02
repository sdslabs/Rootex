
.. _program_listing_file_rootex_framework_components_physics_static_mesh_collider_component.h:

Program Listing for File static_mesh_collider_component.h
=========================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_physics_static_mesh_collider_component.h>` (``rootex\framework\components\physics\static_mesh_collider_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "physics_collider_component.h"
   #include "resource_files/collision_model_resource_file.h"
   
   #include "btBulletCollisionCommon.h"
   
   class StaticMeshColliderComponent : public PhysicsColliderComponent
   {
       static Component* Create(const JSON::json& staticMeshComponentData);
   
       Ref<btBvhTriangleMeshShape> m_MeshShape;
       CollisionModelResourceFile* m_CollisionModel;
   
       friend class ECSFactory;
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::StaticMeshColliderComponent;
   
       StaticMeshColliderComponent(
           const String& collisionModelPath,
           const PhysicsMaterial& material,
           int collisionGroup,
           int collisionMask,
           bool generatesHitEvents);
   
       bool setupData() override;
       void setCollisionModel(CollisionModelResourceFile* file);
   
       virtual const char* getName() const override { return "StaticMeshColliderComponent"; };
       virtual JSON::json getJSON() const override;
       virtual ComponentID getComponentID() const override { return s_ID; }
   
       void draw() override;
   };
