
.. _program_listing_file_rootex_framework_components_audio_listener_component.h:

Program Listing for File audio_listener_component.h
===================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_audio_listener_component.h>` (``rootex\framework\components\audio_listener_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "component.h"
   
   class TransformComponent;
   
   class AudioListenerComponent : public Component
   {
       DEPENDS_ON(TransformComponent);
   
       static Component* Create(const JSON::json& componentData);
   
       friend class ECSFactory;
   
       AudioListenerComponent();
       AudioListenerComponent(const AudioListenerComponent&) = delete;
       ~AudioListenerComponent() = default;
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::AudioListenerComponent;
   
       void onRemove() override;
   
       Vector3 getPosition() const;
   
       virtual const char* getName() const override { return "AudioListenerComponent"; }
       ComponentID getComponentID() const { return s_ID; }
   };
