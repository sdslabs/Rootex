
.. _program_listing_file_game_systems_debug_system.h:

Program Listing for File debug_system.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_game_systems_debug_system.h>` (``game\systems\debug_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "system.h"
   
   class Scene;
   
   class DebugSystem : public System
   {
       bool m_IsEditing = false;
       Vector<float> m_FPSRecords;
   
       DebugSystem();
       DebugSystem(DebugSystem&) = delete;
       ~DebugSystem() = default;
   
       void drawSceneTree(Scene* scene);
   
   public:
       static DebugSystem* GetSingleton();
   
       bool initialize(const JSON::json& systemData) override;
       void update(float deltaMilliseconds) override;
   };
