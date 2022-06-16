
.. _program_listing_file_rootex_framework_components_visual_light_point_light_component.h:

Program Listing for File point_light_component.h
================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_light_point_light_component.h>` (``rootex\framework\components\visual\light\point_light_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "core/renderer/point_light.h"
   #include "component.h"
   #include "components/space/transform_component.h"
   
   class PointLightComponent : public Component
   {
       COMPONENT(PointLightComponent, Category::Light);
       DEPENDS_ON(TransformComponent);
   
       PointLight m_PointLight;
   
   public:
       PointLightComponent::PointLightComponent(Entity& owner, const JSON::json& data);
       ~PointLightComponent() = default;
   
       Matrix getAbsoluteTransform() { return getTransformComponent()->getAbsoluteTransform(); }
       const PointLight& getPointLight() const { return m_PointLight; }
   
       JSON::json getJSON() const override;
       void draw() override;
   };
   
   DECLARE_COMPONENT(PointLightComponent);
