
.. _program_listing_file_rootex_framework_components_visual_light_spot_light_component.h:

Program Listing for File spot_light_component.h
===============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_light_spot_light_component.h>` (``rootex\framework\components\visual\light\spot_light_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "component.h"
   #include "common/common.h"
   
   #include "components/space/transform_component.h"
   #include "core/renderer/spot_light.h"
   
   class SpotLightComponent : public Component
   {
       COMPONENT(SpotLightComponent, Category::Light);
       DEPENDS_ON(TransformComponent);
   
       SpotLight m_SpotLight;
   
   public:
       SpotLightComponent::SpotLightComponent(Entity& owner, const JSON::json& data);
       ~SpotLightComponent() = default;
   
       Matrix getAbsoluteTransform() { return getTransformComponent()->getAbsoluteTransform(); }
       const SpotLight& getSpotLight() const { return m_SpotLight; }
   
       JSON::json getJSON() const override;
       void draw() override;
   };
   
   DECLARE_COMPONENT(SpotLightComponent);
