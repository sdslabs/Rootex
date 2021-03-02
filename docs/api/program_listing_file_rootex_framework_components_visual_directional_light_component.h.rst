
.. _program_listing_file_rootex_framework_components_visual_directional_light_component.h:

Program Listing for File directional_light_component.h
======================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_directional_light_component.h>` (``rootex\framework\components\visual\directional_light_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "component.h"
   #include "common/common.h"
   
   #include "components/transform_component.h"
   #include "core/renderer/directional_light.h"
   
   class DirectionalLightComponent : public Component
   {
       DEPENDS_ON(TransformComponent);
   
       static Component* Create(const JSON::json& componentData);
   
       DirectionalLightComponent(const float diffuseIntensity,
           const Color& diffuseColor, const Color& ambientColor);
       DirectionalLightComponent(DirectionalLightComponent&) = delete;
       ~DirectionalLightComponent() = default;
   
       friend class ECSFactory;
   
       DirectionalLight m_DirectionalLight;
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::DirectionalLightComponent;
   
       Vector3 getDirection() const { return m_TransformComponent->getAbsoluteTransform().Forward(); }
       const DirectionalLight& getDirectionalLight() const { return m_DirectionalLight; }
   
       virtual const char* getName() const override { return "DirectionalLightComponent"; }
       ComponentID getComponentID() const override { return s_ID; }
   
       virtual JSON::json getJSON() const override;
   
       void draw() override;
   };
