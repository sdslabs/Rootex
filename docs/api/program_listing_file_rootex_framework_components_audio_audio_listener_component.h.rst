
.. _program_listing_file_rootex_framework_components_audio_audio_listener_component.h:

Program Listing for File audio_listener_component.h
===================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_audio_audio_listener_component.h>` (``rootex\framework\components\audio\audio_listener_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "component.h"
   #include "components/space/transform_component.h"
   #include "components/physics/box_collider_component.h"
   #include "components/physics/sphere_collider_component.h"
   #include "components/physics/capsule_collider_component.h"
   
   class AudioListenerComponent : public Component
   {
       COMPONENT(AudioListenerComponent, Category::Audio);
   
       DEPENDS_ON(TransformComponent);
       SOFT_DEPENDS_ON(CapsuleColliderComponent);
       SOFT_DEPENDS_ON(BoxColliderComponent);
       SOFT_DEPENDS_ON(SphereColliderComponent);
   
       float m_Volume;
   
   public:
       AudioListenerComponent(Entity& owner, const JSON::json& data);
       ~AudioListenerComponent() = default;
   
       void update();
   
       Vector3 getPosition();
       Vector3 getUp();
       Vector3 getAt();
       RigidBodyComponent* getCollider();
   
       JSON::json getJSON() const override;
       void onRemove() override;
       void draw() override;
   };
   
   DECLARE_COMPONENT(AudioListenerComponent);
