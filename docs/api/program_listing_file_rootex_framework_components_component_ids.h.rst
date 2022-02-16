
.. _program_listing_file_rootex_framework_components_component_ids.h:

Program Listing for File component_ids.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_component_ids.h>` (``rootex\framework\components\component_ids.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   enum class ComponentIDs : unsigned int
   {
       ModelComponent,
       GridModelComponent,
       SpriteComponent,
       CPUParticlesComponent,
       DecalComponent,
       TextUIComponent,
       UIComponent,
       CameraComponent,
       TransformComponent,
       PointLightComponent,
       DirectionalLightComponent,
       SpotLightComponent,
       PhysicsColliderComponent,
       SphereColliderComponent,
       BoxColliderComponent,
       CapsuleColliderComponent,
       StaticMeshColliderComponent,
       TriggerComponent,
       ScriptComponent,
       MusicComponent,
       ShortMusicComponent,
       AudioListenerComponent,
       TransformAnimationComponent,
       SkyComponent,
       FogComponent,
       StaticPointLightComponent,
       AnimatedModelComponent,
       RenderableComponent,
       ParticleEffectComponent,
       PlayerController
   };
