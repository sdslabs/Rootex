
.. _program_listing_file_rootex_framework_systems_player_system.h:

Program Listing for File player_system.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_systems_player_system.h>` (``rootex\framework\systems\player_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "framework/system.h"
   
   class PlayerSystem : public System
   {
       PlayerSystem();
       PlayerSystem(const PlayerSystem&) = delete;
       virtual ~PlayerSystem() = default;
   
   public:
       static PlayerSystem* GetSingleton();
   
       bool initialize(const JSON::json& systemData) override;
       void setConfig(const SceneSettings& sceneSettings) override;
   
       void begin() override;
       void update(float deltaMilliseconds) override;
       void end() override;
   };
