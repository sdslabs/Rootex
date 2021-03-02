
.. _program_listing_file_rootex_framework_systems_animation_system.h:

Program Listing for File animation_system.h
===========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_systems_animation_system.h>` (``rootex\framework\systems\animation_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "system.h"
   
   class AnimationSystem : public System
   {
       AnimationSystem();
       AnimationSystem(AnimationSystem&) = delete;
       ~AnimationSystem() = default;
   
   public:
       static AnimationSystem* GetSingleton();
   
       void update(float deltaMilliseconds) override;
   };
