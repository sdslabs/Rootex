
.. _program_listing_file_rootex_core_renderer_mesh.h:

Program Listing for File mesh.h
===============================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_mesh.h>` (``rootex\core\renderer\mesh.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/types.h"
   
   #define MAX_LOD_COUNT 5
   
   class VertexBuffer;
   class IndexBuffer;
   
   struct Mesh
   {
       Ref<VertexBuffer> m_VertexBuffer;
       BoundingBox m_BoundingBox;
   
       Vector<Pair<Ref<IndexBuffer>, float>> m_LODs;
   
       Mesh() = default;
       Mesh(const Mesh&) = default;
       ~Mesh() = default;
   
       void addLOD(Ref<IndexBuffer> ib, float lodLevel);
   
       Ref<IndexBuffer> getLOD(float lodLevel) const;
       Ref<VertexBuffer> getVertexBuffer() { return m_VertexBuffer; };
       const BoundingBox& getBoundingBox() { return m_BoundingBox; };
   };
