
.. _program_listing_file_rootex_framework_components_visual_static_point_light_component.h:

Program Listing for File static_point_light_component.h
=======================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_static_point_light_component.h>` (``rootex\framework\components\visual\static_point_light_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "component.h"
   
   #include "point_light_component.h"
   
   class StaticPointLightComponent : public PointLightComponent
   {
       static Component* Create(const JSON::json& componentData);
   
       StaticPointLightComponent::StaticPointLightComponent(const float constAtt, const float linAtt, const float quadAtt,
           const float range, const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor);
       StaticPointLightComponent(StaticPointLightComponent&) = delete;
       ~StaticPointLightComponent() = default;
   
       friend class ECSFactory;
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::StaticPointLightComponent;
   
       virtual const char* getName() const override { return "StaticPointLightComponent"; }
       ComponentID getComponentID() const override { return s_ID; }
       virtual JSON::json getJSON() const override;
   
       void draw() override;
   };
