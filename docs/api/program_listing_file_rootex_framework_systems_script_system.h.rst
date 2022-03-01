
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
       ScriptSystem(const ScriptSystem&) = delete;
       ~ScriptSystem() = default;
   
       Vector<Entity*> m_ScriptEntitiesToInit;
       Vector<Entity*> m_ScriptEntitiesToEnter;
   
       sol::table m_FluxTweener;
   
   public:
       static ScriptSystem* GetSingleton();
   
       void addInitScriptEntity(Entity* e);
       void removeInitScriptEntity(Entity* e);
   
       void addEnterScriptEntity(Entity* e);
       void removeEnterScriptEntity(Entity* e);
   
       void update(float deltaMilliseconds) override;
       void end() override;
   };
