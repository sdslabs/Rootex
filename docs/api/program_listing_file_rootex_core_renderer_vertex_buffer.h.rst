
.. _program_listing_file_rootex_core_renderer_vertex_buffer.h:

Program Listing for File vertex_buffer.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_vertex_buffer.h>` (``rootex\core\renderer\vertex_buffer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <d3d11.h>
   
   #include "common/common.h"
   #include "renderer/vertex_data.h"
   
   class VertexBuffer
   {
       Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
       unsigned int m_Stride;
       unsigned int m_Count;
   
   public:
       VertexBuffer(const char* buffer, unsigned int elementCount, unsigned int stride, D3D11_USAGE usage, int cpuAccess);
       ~VertexBuffer() = default;
   
       void bind() const;
   
       unsigned int getCount() const { return m_Count; }
       unsigned int getStride() const { return m_Stride; }
       ID3D11Buffer* getBuffer() const { return m_VertexBuffer.Get(); };
   };
