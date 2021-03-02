
.. _program_listing_file_rootex_framework_systems_light_system.h:

Program Listing for File light_system.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_systems_light_system.h>` (``rootex\framework\systems\light_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "system.h"
   #include "renderer/constant_buffer.h"
   
   class LightSystem : public System
   {
       LightSystem();
   
   public:
       static LightSystem* GetSingleton();
   
       StaticPointLightsInfo getStaticPointLights();
       LightsInfo getDynamicLights();
   };
