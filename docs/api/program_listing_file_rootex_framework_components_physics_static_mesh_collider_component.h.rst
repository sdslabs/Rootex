
.. _program_listing_file_rootex_framework_components_physics_static_mesh_collider_component.h:

Program Listing for File static_mesh_collider_component.h
=========================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_physics_static_mesh_collider_component.h>` (``rootex\framework\components\physics\static_mesh_collider_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "rigid_body_component.h"
   #include "resource_files/collision_model_resource_file.h"
   
   #include "btBulletCollisionCommon.h"
   
   class StaticMeshColliderComponent : public RigidBodyComponent
   {
       COMPONENT(StaticMeshColliderComponent, Category::Physics);
   
       btBvhTriangleMeshShape* m_MeshShape;
       Ref<CollisionModelResourceFile> m_CollisionModel;
   
       void createStaticMesh();
   
   public:
       StaticMeshColliderComponent(Entity& owner, const JSON::json& data);
       ~StaticMeshColliderComponent() = default;
   
       bool setupData() override;
       void setCollisionModel(Ref<CollisionModelResourceFile> file);
   
       JSON::json getJSON() const override;
       void draw() override;
   };
   
   DECLARE_COMPONENT(StaticMeshColliderComponent);
