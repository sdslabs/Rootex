
.. _program_listing_file_rootex_core_renderer_mesh.h:

Program Listing for File mesh.h
===============================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_mesh.h>` (``rootex\core\renderer\mesh.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "index_buffer.h"
   #include "vertex_buffer.h"
   
   class BasicMaterial;
   
   struct Mesh
   {
       Ref<VertexBuffer> m_VertexBuffer;
       Ref<IndexBuffer> m_IndexBuffer;
       BoundingBox m_BoundingBox;
   
       Mesh() = default;
       Mesh(const Mesh&) = default;
       ~Mesh() = default;
   };
