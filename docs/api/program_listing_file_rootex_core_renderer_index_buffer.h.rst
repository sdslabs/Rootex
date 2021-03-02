
.. _program_listing_file_rootex_core_renderer_index_buffer.h:

Program Listing for File index_buffer.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_index_buffer.h>` (``rootex\core\renderer\index_buffer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <d3d11.h>
   
   #include "common/common.h"
   
   class IndexBuffer
   {
   protected:
       Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;
       unsigned int m_Count;
       DXGI_FORMAT m_Format;
   
   public:
       IndexBuffer(const Vector<unsigned short>& indices);
       IndexBuffer(const Vector<unsigned int>& indices);
       ~IndexBuffer() = default;
   
       void setData(const Vector<unsigned short>& indices);
       void setData(const Vector<unsigned int>& indices);
   
       void bind() const;
       unsigned int getCount() const;
       ID3D11Buffer* getBuffer() { return m_IndexBuffer.Get(); }
   };
