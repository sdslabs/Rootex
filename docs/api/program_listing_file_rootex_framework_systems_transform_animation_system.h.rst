
.. _program_listing_file_rootex_framework_systems_transform_animation_system.h:

Program Listing for File transform_animation_system.h
=====================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_systems_transform_animation_system.h>` (``rootex\framework\systems\transform_animation_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "system.h"
   
   class TransformAnimationSystem : public System
   {
   public:
       static TransformAnimationSystem* GetSingleton();
   
       TransformAnimationSystem();
   
       void begin();
       void update(float deltaMilliseconds) override;
   };
