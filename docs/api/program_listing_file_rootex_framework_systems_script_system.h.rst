
.. _program_listing_file_rootex_framework_systems_script_system.h:

Program Listing for File script_system.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_systems_script_system.h>` (``rootex\framework\systems\script_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "system.h"
   
   class ScriptSystem : public System
   {
       ScriptSystem();
       ScriptSystem(ScriptSystem&) = delete;
       ~ScriptSystem() = default;
   
   public:
       static ScriptSystem* GetSingleton();
   
       void begin() override;
       void update(float deltaMilliseconds) override;
       void end() override;
   };
