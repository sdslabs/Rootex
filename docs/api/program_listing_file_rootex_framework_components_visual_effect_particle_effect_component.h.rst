
.. _program_listing_file_rootex_framework_components_visual_effect_particle_effect_component.h:

Program Listing for File particle_effect_component.h
====================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_effect_particle_effect_component.h>` (``rootex\framework\components\visual\effect\particle_effect_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/types.h"
   #include "components/space/transform_component.h"
   #include "resource_files/particle_effect_resource_file.h"
   
   #include "Effekseer.h"
   
   class ParticleEffectComponent : public Component
   {
       COMPONENT(ParticleEffectComponent, Category::Effect);
       DEPENDS_ON(TransformComponent);
   
       Effekseer::Handle m_EffectHandle;
   
       Ref<ParticleEffectResourceFile> m_EffectResource;
       int m_StartFrame;
       bool m_PlayOnStart;
       bool m_IsMoving;
       bool m_UseSpeed;
       float m_Speed;
       bool m_UseTarget;
       Vector3 m_TargetLocation;
   
   public:
       ParticleEffectComponent(Entity& owner, const JSON::json& data);
       ~ParticleEffectComponent() = default;
   
       bool isPlayOnStart() const { return m_PlayOnStart; }
   
       bool isMoving() const { return m_IsMoving; }
       void setMoving(bool enabled) { m_IsMoving = enabled; }
   
       bool isPaused() const;
       void setPlaying(bool enabled);
       void play();
       void stop();
   
       ParticleEffectResourceFile* getEffectResource() { return m_EffectResource.get(); }
       void setEffect(Ref<ParticleEffectResourceFile> effect);
   
       Effekseer::Handle getHandle() const { return m_EffectHandle; }
   
       JSON::json getJSON() const override;
       void draw() override;
   };
   
   DECLARE_COMPONENT(ParticleEffectComponent);
