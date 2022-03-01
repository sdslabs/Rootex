
.. _program_listing_file_rootex_framework_systems_trigger_system.h:

Program Listing for File trigger_system.h
=========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_systems_trigger_system.h>` (``rootex\framework\systems\trigger_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "system.h"
   
   class TriggerSystem : public System
   {
       TriggerSystem();
       TriggerSystem(const TriggerSystem&) = delete;
       ~TriggerSystem() = default;
   
   public:
       static TriggerSystem* GetSingleton();
   
       void update(float deltaMilliseconds) override;
   };
