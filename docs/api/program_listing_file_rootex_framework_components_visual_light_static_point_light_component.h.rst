
.. _program_listing_file_rootex_framework_components_visual_light_static_point_light_component.h:

Program Listing for File static_point_light_component.h
=======================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_light_static_point_light_component.h>` (``rootex\framework\components\visual\light\static_point_light_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "component.h"
   
   #include "point_light_component.h"
   
   class StaticPointLightComponent : public PointLightComponent
   {
       COMPONENT(StaticPointLightComponent, Category::Light);
   
   public:
       StaticPointLightComponent::StaticPointLightComponent(Entity& owner, const JSON::json& data);
       ~StaticPointLightComponent() = default;
   
       void draw() override;
   };
   
   DECLARE_COMPONENT(StaticPointLightComponent);
