
.. _program_listing_file_rootex_framework_components_visual_model_sprite_component.h:

Program Listing for File sprite_component.h
===========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_model_sprite_component.h>` (``rootex\framework\components\visual\model\sprite_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "model_component.h"
   #include "core/resource_files/image_resource_file.h"
   #include "core/resource_files/basic_material_resource_file.h"
   
   class SpriteComponent : public ModelComponent
   {
       COMPONENT(SpriteComponent, Category::Model);
   
   private:
       bool m_IsBillboarded;
   
       void adjustScaling();
       void setMaterialOverride(MaterialResourceFile* oldMaterial, Ref<MaterialResourceFile> newMaterial) override;
   
       static JSON::json InjectSpriteModel(const JSON::json& data);
   
   public:
       SpriteComponent(Entity& owner, const JSON::json& data);
       ~SpriteComponent() = default;
   
       Ref<MaterialResourceFile> getOverridingMaterialResourceFile();
   
       bool preRender(float deltaMilliseconds) override;
       void postRender() override;
   
       bool setupData() override;
       JSON::json getJSON() const override;
       void draw() override;
   };
   
   DECLARE_COMPONENT(SpriteComponent);
