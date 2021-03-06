
.. _program_listing_file_rootex_framework_components_physics_box_collider_component.h:

Program Listing for File box_collider_component.h
=================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_physics_box_collider_component.h>` (``rootex\framework\components\physics\box_collider_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "physics_collider_component.h"
   
   #include "btBulletDynamicsCommon.h"
   
   class BoxColliderComponent : public PhysicsColliderComponent
   {
       static Component* Create(const JSON::json& boxComponentData);
   
       Vector3 m_Dimensions;
       Ref<btBoxShape> m_BoxShape;
   
       friend class ECSFactory;
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::BoxColliderComponent;
   
       BoxColliderComponent(
           const Vector3& dimensions,
           const PhysicsMaterial& material,
           const Vector3& angularFactor,
           const Vector3& gravity,
           int collisionGroup,
           int collisionMask,
           bool isMoveable,
           bool isKinematic,
           bool generatesHitEvents,
           bool isSleepable,
           bool isCCD);
   
       Vector3 getDimensions() const { return m_Dimensions; }
       virtual const char* getName() const override { return "BoxColliderComponent"; };
       virtual JSON::json getJSON() const override;
       virtual ComponentID getComponentID() const override { return s_ID; }
   
       void setDimensions(const Vector3& dimensions);
   
       void draw() override;
   };
