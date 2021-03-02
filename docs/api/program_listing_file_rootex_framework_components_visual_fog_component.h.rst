
.. _program_listing_file_rootex_framework_components_visual_fog_component.h:

Program Listing for File fog_component.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_fog_component.h>` (``rootex\framework\components\visual\fog_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "component.h"
   
   class FogComponent : public Component
   {
       static Component* Create(const JSON::json& componentData);
   
       friend class ECSFactory;
   
       float m_Near;
       float m_Far;
       Color m_Color;
   
       FogComponent(float nearDistance, float farDistance, const Color& color);
       FogComponent(const FogComponent&) = delete;
       FogComponent() = default;
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::FogComponent;
   
       Color getColor() const { return m_Color; }
       float getNearDistance() const { return m_Near; }
       float getFarDistance() const { return m_Far; }
   
       virtual const char* getName() const override { return "FogComponent"; }
       ComponentID getComponentID() const { return s_ID; }
       virtual JSON::json getJSON() const override;
   
       void draw() override;
   };
