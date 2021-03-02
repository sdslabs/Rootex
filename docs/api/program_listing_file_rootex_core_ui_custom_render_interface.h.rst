
.. _program_listing_file_rootex_core_ui_custom_render_interface.h:

Program Listing for File custom_render_interface.h
==================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_ui_custom_render_interface.h>` (``rootex\core\ui\custom_render_interface.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "core/renderer/material_library.h"
   #include "event_manager.h"
   
   #undef interface
   #include "RmlUi/Core.h"
   #define interface __STRUCT__
   
   class CustomRenderInterface : public Rml::RenderInterface
   {
       static unsigned int s_TextureCount;
   
       Ref<Shader> m_UIShader;
       HashMap<unsigned int, Ref<Texture>> m_Textures;
       Microsoft::WRL::ComPtr<ID3D11Buffer> m_ModelMatrixBuffer;
       Matrix m_UITransform;
       int m_Width;
       int m_Height;
   
       Variant windowResized(const Event* event);
   
   public:
       CustomRenderInterface(int width, int height);
       CustomRenderInterface(const CustomRenderInterface&) = delete;
       virtual ~CustomRenderInterface() = default;
   
       virtual void RenderGeometry(Rml::Vertex* vertices, int numVertices, int* indices, int numIndices, Rml::TextureHandle texture, const Rml::Vector2f& translation) override;
   
       virtual Rml::CompiledGeometryHandle CompileGeometry(Rml::Vertex* vertices, int numVertices, int* indices, int numIndices, Rml::TextureHandle texture) override;
       virtual void RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation) override;
       virtual void ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry) override;
   
       virtual bool LoadTexture(Rml::TextureHandle& textureHandle, Rml::Vector2i& textureDimensions, const String& source) override;
       virtual bool GenerateTexture(Rml::TextureHandle& textureHandle, const byte* source, const Rml::Vector2i& sourceDimensions) override;
       virtual void ReleaseTexture(Rml::TextureHandle texture);
   
       virtual void EnableScissorRegion(bool enable) override;
       virtual void SetScissorRegion(int x, int y, int width, int height) override;
   
       virtual void SetTransform(const Rml::Matrix4f* transform) override;
   };
