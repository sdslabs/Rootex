
.. _program_listing_file_rootex_framework_components_visual_effect_sky_component.h:

Program Listing for File sky_component.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_effect_sky_component.h>` (``rootex\framework\components\visual\effect\sky_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "component.h"
   #include "core/resource_files/model_resource_file.h"
   #include "core/resource_files/sky_material_resource_file.h"
   
   class SkyComponent : public Component
   {
       COMPONENT(SkyComponent, Category::Effect);
   
       Ref<ModelResourceFile> m_SkySphere;
       Ref<SkyMaterialResourceFile> m_SkyMaterial;
   
   public:
       SkyComponent(Entity& owner, const JSON::json& data);
       ~SkyComponent() = default;
   
       ModelResourceFile* getSkySphere() const { return m_SkySphere.get(); }
       SkyMaterialResourceFile* getSkyMaterial() const { return m_SkyMaterial.get(); }
   
       JSON::json getJSON() const override;
       void draw() override;
   };
   
   DECLARE_COMPONENT(SkyComponent);
