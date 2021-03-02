
.. _program_listing_file_rootex_framework_components_visual_spot_light_component.h:

Program Listing for File spot_light_component.h
===============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_spot_light_component.h>` (``rootex\framework\components\visual\spot_light_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "component.h"
   #include "common/common.h"
   
   #include "components/transform_component.h"
   #include "core/renderer/spot_light.h"
   
   class SpotLightComponent : public Component
   {
       DEPENDS_ON(TransformComponent);
   
       static Component* Create(const JSON::json& componentData);
   
       SpotLightComponent::SpotLightComponent(const float constAtt, const float linAtt, const float quadAtt,
           const float range, const float diffuseIntensity, const Color& diffuseColor, const Color& ambientColor,
           float spot, float angleRange);
       SpotLightComponent(SpotLightComponent&) = delete;
       ~SpotLightComponent() = default;
   
       friend class ECSFactory;
   
       SpotLight m_SpotLight;
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::SpotLightComponent;
   
       Matrix getAbsoluteTransform() const { return m_TransformComponent->getAbsoluteTransform(); }
       const SpotLight& getSpotLight() const { return m_SpotLight; }
   
       virtual const char* getName() const override { return "SpotLightComponent"; }
       ComponentID getComponentID() const override { return s_ID; }
   
       virtual JSON::json getJSON() const override;
   
       void draw() override;
   };
