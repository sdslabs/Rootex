
.. _program_listing_file_rootex_framework_components_game_player_controller.h:

Program Listing for File player_controller.h
============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_game_player_controller.h>` (``rootex\framework\components\game\player_controller.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "framework/component.h"
   #include "framework/components/space/transform_component.h"
   #include "framework/components/visual/model/animated_model_component.h"
   #include "framework/components/physics/capsule_collider_component.h"
   #include "core/state_manager.h"
   
   class PlayerController : public Component
   {
       COMPONENT(PlayerController, Category::Game);
       DEPENDS_ON(TransformComponent);
       DEPENDS_ON(AnimatedModelComponent);
       DEPENDS_ON(CapsuleColliderComponent);
   
   public:
       String m_WalkAnimation;
       String m_RunAnimation;
       String m_IdleAnimation;
       String m_TurnLeftAnimation;
       String m_TurnRightAnimation;
       float m_MaxWalkSpeed;
       float m_MaxRunSpeed;
       float m_StoppingPower;
       float m_IdleThreshold;
   
       Ref<StateManager> m_StateManager;
       float m_Acceleration;
       Vector3 m_Velocity;
   
       PlayerController(Entity& owner, const JSON::json& data);
       ~PlayerController() = default;
   
       void update(float deltaMilliseconds);
   
       bool setupData() override;
       JSON::json getJSON() const override;
       void draw() override;
       void drawAnimation(const char* animation, String& editing);
   };
   
   DECLARE_COMPONENT(PlayerController);
