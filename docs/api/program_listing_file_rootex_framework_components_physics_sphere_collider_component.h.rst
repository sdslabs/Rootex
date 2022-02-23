
.. _program_listing_file_rootex_framework_components_physics_sphere_collider_component.h:

Program Listing for File sphere_collider_component.h
====================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_physics_sphere_collider_component.h>` (``rootex\framework\components\physics\sphere_collider_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "rigid_body_component.h"
   
   #include "btBulletDynamicsCommon.h"
   
   class SphereColliderComponent : public RigidBodyComponent
   {
       COMPONENT(SphereColliderComponent, Category::Physics);
   
       float m_Radius;
       btSphereShape* m_SphereShape;
   
   public:
       SphereColliderComponent(Entity& owner, const JSON::json& data);
       ~SphereColliderComponent() = default;
   
       float getRadius() const { return m_Radius; }
       void setRadius(float r);
   
       JSON::json getJSON() const override;
       void draw() override;
   };
   
   DECLARE_COMPONENT(SphereColliderComponent);
