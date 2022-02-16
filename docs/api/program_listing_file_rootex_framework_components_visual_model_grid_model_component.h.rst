
.. _program_listing_file_rootex_framework_components_visual_model_grid_model_component.h:

Program Listing for File grid_model_component.h
===============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_model_grid_model_component.h>` (``rootex\framework\components\visual\model\grid_model_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "model_component.h"
   #include "renderer/render_pass.h"
   #include "core/resource_files/basic_material_resource_file.h"
   
   class GridModelComponent : public ModelComponent
   {
       COMPONENT(GridModelComponent, Category::Model);
   
       Ref<BasicMaterialResourceFile> m_ColorMaterial;
       Ref<VertexBuffer> m_VertexBuffer;
       Ref<IndexBuffer> m_IndexBuffer;
   
       Vector2 m_CellSize;
       int m_CellCount;
   
       void refreshVertexBuffers();
   
   public:
       GridModelComponent(Entity& owner, const JSON::json& data);
       ~GridModelComponent() = default;
   
       void render(float viewDistance) override;
   
       bool setupData() override;
       JSON::json getJSON() const override;
       void draw() override;
   };
   
   DECLARE_COMPONENT(GridModelComponent);
