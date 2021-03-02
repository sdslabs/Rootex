
.. _program_listing_file_rootex_core_renderer_renderer.h:

Program Listing for File renderer.h
===================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_renderer.h>` (``rootex\core\renderer\renderer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "vertex_buffer.h"
   #include "index_buffer.h"
   #include "material.h"
   #include "rendering_device.h"
   #include "viewport.h"
   
   class Renderer
   {
       Shader* m_CurrentShader;
   
   public:
       Renderer();
       Renderer(const Renderer&) = delete;
       Renderer& operator=(const Renderer&) = delete;
       virtual ~Renderer() = default;
   
       void setViewport(Viewport& viewport);
   
       void resetCurrentShader();
       void bind(Material* material);
       void draw(const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer) const;
       void drawInstanced(const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer, const VertexBuffer* instanceBuffer, unsigned int instances) const;
   };
