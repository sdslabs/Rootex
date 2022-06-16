
.. _program_listing_file_rootex_framework_components_visual_effect_decal_component.h:

Program Listing for File decal_component.h
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_effect_decal_component.h>` (``rootex\framework\components\visual\effect\decal_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "components/visual/model/model_component.h"
   #include "resource_files/decal_material_resource_file.h"
   
   class DecalComponent : public ModelComponent
   {
       COMPONENT(DecalComponent, Category::Effect);
   
   private:
       Microsoft::WRL::ComPtr<ID3D11Buffer> m_PerDecalCB;
   
       Ref<DecalMaterialResourceFile> m_DecalMaterialResourceFile;
   
       void bindDepthTexture();
       void bindDecalPSCB();
   
   public:
       DecalComponent(Entity& owner, const JSON::json& data);
       ~DecalComponent() = default;
   
       void render(float viewDistance) override;
   
       JSON::json getJSON() const override;
       void draw() override;
   };
   
   DECLARE_COMPONENT(DecalComponent);
