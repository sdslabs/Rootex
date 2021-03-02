
.. _program_listing_file_rootex_core_renderer_buffer_format.h:

Program Listing for File buffer_format.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_buffer_format.h>` (``rootex\core\renderer\buffer_format.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <dxgi.h>
   
   #include "common/common.h"
   
   struct VertexBufferElement
   {
       enum Type
       {
           FloatFloatFloatFloat = DXGI_FORMAT_R32G32B32A32_FLOAT,
           FloatFloatFloat = DXGI_FORMAT_R32G32B32_FLOAT,
           FloatFloat = DXGI_FORMAT_R32G32_FLOAT,
           ByteByteByteByte = DXGI_FORMAT_R8G8B8A8_UNORM,
           UInt = DXGI_FORMAT_R32_UINT,
           IntIntIntInt = DXGI_FORMAT_R32G32B32A32_SINT,
       };
   
       Type m_Type;
       LPCSTR m_Name;
       D3D11_INPUT_CLASSIFICATION m_Class;
       int m_Slot;
       bool m_ResetOffset;
       UINT m_RendersPerInstance;
   
       static unsigned int GetSize(Type type)
       {
           switch (type)
           {
           case FloatFloatFloatFloat:
               return sizeof(float) * 4;
           case FloatFloatFloat:
               return sizeof(float) * 3;
           case FloatFloat:
               return sizeof(float) * 2;
           case ByteByteByteByte:
               return sizeof(char) * 4;
           case IntIntIntInt:
               return sizeof(int) * 4;
           case UInt:
               return sizeof(unsigned int);
           default:
               ERR("Unknown size found");
               return 0;
           }
       }
   };
   
   class BufferFormat
   {
       Vector<VertexBufferElement> m_Elements;
   
   public:
       BufferFormat() = default;
   
       void push(VertexBufferElement::Type type, LPCSTR name, D3D11_INPUT_CLASSIFICATION elementClass, int slot, bool resetOffset, UINT rendersPerInstance) { m_Elements.push_back(VertexBufferElement { type, name, elementClass, slot, resetOffset, rendersPerInstance }); }
   
       const Vector<VertexBufferElement>& getElements() const { return m_Elements; }
   };
