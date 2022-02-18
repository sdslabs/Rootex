
.. _program_listing_file_rootex_framework_components_physics_capsule_collider_component.h:

Program Listing for File capsule_collider_component.h
=====================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_physics_capsule_collider_component.h>` (``rootex\framework\components\physics\capsule_collider_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "rigid_body_component.h"
   
   #include "btBulletDynamicsCommon.h"
   
   class CapsuleColliderComponent : public RigidBodyComponent
   {
       COMPONENT(CapsuleColliderComponent, Category::Physics);
   
       float m_Radius;
       float m_SideHeight;
       btCapsuleShape* m_CapsuleShape;
   
   public:
       CapsuleColliderComponent(Entity& owner, const JSON::json& data);
       ~CapsuleColliderComponent() = default;
   
       float getSideHeight() const { return m_SideHeight; }
       void setSideHeight(float s);
   
       float getRadius() const { return m_Radius; }
       void setRadius(float r);
   
       JSON::json getJSON() const override;
       void draw() override;
   };
   
   DECLARE_COMPONENT(CapsuleColliderComponent);
