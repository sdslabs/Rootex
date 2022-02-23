
.. _program_listing_file_rootex_framework_components_visual_effect_fog_component.h:

Program Listing for File fog_component.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_effect_fog_component.h>` (``rootex\framework\components\visual\effect\fog_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "component.h"
   
   class FogComponent : public Component
   {
       COMPONENT(FogComponent, Category::Effect);
   
       float m_Near;
       float m_Far;
       Color m_Color;
   
   public:
       FogComponent(Entity& owner, const JSON::json& data);
       FogComponent() = default;
   
       Color getColor() const { return m_Color; }
       float getNearDistance() const { return m_Near; }
       float getFarDistance() const { return m_Far; }
   
       JSON::json getJSON() const override;
       void draw() override;
   };
   
   DECLARE_COMPONENT(FogComponent);
