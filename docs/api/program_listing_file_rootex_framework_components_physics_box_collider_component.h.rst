
.. _program_listing_file_rootex_framework_components_physics_box_collider_component.h:

Program Listing for File box_collider_component.h
=================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_physics_box_collider_component.h>` (``rootex\framework\components\physics\box_collider_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "rigid_body_component.h"
   
   #include "btBulletDynamicsCommon.h"
   
   class BoxColliderComponent : public RigidBodyComponent
   {
       COMPONENT(BoxColliderComponent, Category::Physics);
   
       Vector3 m_Dimensions;
       btBoxShape* m_BoxShape;
   
   public:
       BoxColliderComponent(Entity& owner, const JSON::json& data);
       ~BoxColliderComponent() = default;
   
       void setDimensions(const Vector3& dimensions);
       Vector3 getDimensions() const { return m_Dimensions; }
   
       JSON::json getJSON() const override;
       void draw() override;
   };
   
   DECLARE_COMPONENT(BoxColliderComponent);
