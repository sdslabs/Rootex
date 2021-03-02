
.. _program_listing_file_rootex_framework_components_visual_point_light_component.h:

Program Listing for File point_light_component.h
================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_point_light_component.h>` (``rootex\framework\components\visual\point_light_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "component.h"
   #include "components/transform_component.h"
   #include "core/renderer/point_light.h"
   
   class PointLightComponent : public Component
   {
       DEPENDS_ON(TransformComponent);
   
       static Component* Create(const JSON::json& componentData);
   
       PointLight m_PointLight;
   
   protected:
       PointLightComponent::PointLightComponent(const float constAtt, const float linAtt, const float quadAtt,
           const float range, const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor);
       PointLightComponent(PointLightComponent&) = delete;
       ~PointLightComponent() = default;
   
       friend class ECSFactory;
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::PointLightComponent;
   
       Matrix getAbsoluteTransform() const { return m_TransformComponent->getAbsoluteTransform(); }
       const PointLight& getPointLight() const { return m_PointLight; }
   
       virtual const char* getName() const override { return "PointLightComponent"; }
       ComponentID getComponentID() const override { return s_ID; }
       virtual JSON::json getJSON() const override;
   
       void draw() override;
   };
