
.. _program_listing_file_rootex_framework_components_visual_grid_model_component.h:

Program Listing for File grid_model_component.h
===============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_grid_model_component.h>` (``rootex\framework\components\visual\grid_model_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "model_component.h"
   #include "renderer/render_pass.h"
   #include "renderer/material_library.h"
   
   class GridModelComponent : public ModelComponent
   {
       static Component* Create(const JSON::json& componentData);
   
       Ref<Material> m_ColorMaterial;
       Ptr<VertexBuffer> m_VertexBuffer;
       Ptr<IndexBuffer> m_IndexBuffer;
   
       Vector2 m_CellSize;
       int m_CellCount;
   
       GridModelComponent(const Vector2& cellSize, const int& cellCount, const unsigned int& renderPass, bool isVisible);
       GridModelComponent(GridModelComponent&) = delete;
       virtual ~GridModelComponent() = default;
   
       friend class ECSFactory;
   
       void refreshVertexBuffers();
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::GridModelComponent;
   
       virtual bool setupData() override;
       void render() override;
   
       virtual const char* getName() const override { return "GridModelComponent"; }
       ComponentID getComponentID() const override { return s_ID; }
       virtual JSON::json getJSON() const override;
   
       void draw() override;
   };
