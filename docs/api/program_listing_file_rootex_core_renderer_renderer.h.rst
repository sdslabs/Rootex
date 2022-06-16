
.. _program_listing_file_rootex_core_renderer_renderer.h:

Program Listing for File renderer.h
===================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_renderer.h>` (``rootex\core\renderer\renderer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "vertex_buffer.h"
   #include "index_buffer.h"
   #include "core/resource_files/material_resource_file.h"
   #include "rendering_device.h"
   #include "shader.h"
   #include "viewport.h"
   
   class Renderer
   {
       const Shader* m_CurrentShader;
   
   public:
       Renderer();
       Renderer(const Renderer&) = delete;
       Renderer& operator=(const Renderer&) = delete;
       virtual ~Renderer() = default;
   
       void setViewport(Viewport& viewport);
   
       void resetCurrentShader();
       void bind(MaterialResourceFile* newMaterial, MaterialResourceFile* oldMaterial);
       void bind(MaterialResourceFile* Material);
       void draw(const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer) const;
       void drawInstanced(const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer, const VertexBuffer* instanceBuffer, unsigned int instances) const;
   };
