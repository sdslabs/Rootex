
.. _program_listing_file_rootex_core_renderer_shader.h:

Program Listing for File shader.h
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_shader.h>` (``rootex\core\renderer\shader.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   
   #include "buffer_format.h"
   #include "constant_buffer.h"
   #include "rendering_device.h"
   #include "texture.h"
   
   class Shader
   {
   public:
       Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
       Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
       Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
   
       bool m_IsValid = true;
   
   public:
       Shader() = default;
       Shader(const String& vertexPath, const String& pixelPath, const BufferFormat& vertexBufferFormat);
       ~Shader() = default;
   
       void bind() const;
   
       bool isValid() const { return m_IsValid; }
   };
