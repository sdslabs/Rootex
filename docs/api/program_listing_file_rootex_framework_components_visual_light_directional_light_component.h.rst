
.. _program_listing_file_rootex_framework_components_visual_light_directional_light_component.h:

Program Listing for File directional_light_component.h
======================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_light_directional_light_component.h>` (``rootex\framework\components\visual\light\directional_light_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "component.h"
   #include "common/common.h"
   #include "components/space/transform_component.h"
   #include "core/renderer/directional_light.h"
   
   class DirectionalLightComponent : public Component
   {
       COMPONENT(DirectionalLightComponent, Category::Light);
       DEPENDS_ON(TransformComponent);
   
       DirectionalLight m_DirectionalLight;
   
   public:
       DirectionalLightComponent(Entity& owner, const JSON::json& data);
       ~DirectionalLightComponent() = default;
   
       Vector3 getDirection() { return getTransformComponent()->getAbsoluteTransform().Forward(); }
       const DirectionalLight& getDirectionalLight() const { return m_DirectionalLight; }
   
       JSON::json getJSON() const override;
       void draw() override;
   };
   
   DECLARE_COMPONENT(DirectionalLightComponent);
