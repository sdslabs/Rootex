
.. _program_listing_file_game_systems_game_render_system.h:

Program Listing for File game_render_system.h
=============================================

|exhale_lsh| :ref:`Return to documentation for file <file_game_systems_game_render_system.h>` (``game\systems\game_render_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "framework/system.h"
   
   #include "PostProcess.h"
   
   class GameRenderSystem : public System
   {
       Ptr<DirectX::BasicPostProcess> m_BasicPostProcess;
   
       GameRenderSystem();
   
   public:
       static GameRenderSystem* GetSingleton();
   
       virtual bool initialize(const JSON::json& systemData) override;
       virtual void update(float deltaMilliseconds) override;
   };
