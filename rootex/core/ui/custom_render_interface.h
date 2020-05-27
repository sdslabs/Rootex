#pragma once

#include "core/renderer/material_library.h"

#undef interface
#include "RmlUi/Core.h"
#define interface __STRUCT__

class CustomRenderInterface : public Rml::Core::RenderInterface
{
	Ref<Shader> m_UIShader;
	HashMap<unsigned int, Ref<Texture>> m_Textures;

public:
	CustomRenderInterface();
	CustomRenderInterface(const CustomRenderInterface&) = delete;
	virtual ~CustomRenderInterface() = default;

	virtual void RenderGeometry(Rml::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::Core::TextureHandle texture, const Rml::Core::Vector2f& translation) override;
	
	virtual Rml::Core::CompiledGeometryHandle CompileGeometry(Rml::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::Core::TextureHandle texture) override;
	virtual void RenderCompiledGeometry(Rml::Core::CompiledGeometryHandle geometry, const Rml::Core::Vector2f& translation) override;
	virtual void ReleaseCompiledGeometry(Rml::Core::CompiledGeometryHandle geometry) override;

	virtual bool LoadTexture(Rml::Core::TextureHandle& texture_handle, Rml::Core::Vector2i& texture_dimensions, const String& source) override;
	virtual bool GenerateTexture(Rml::Core::TextureHandle& texture_handle, const byte* source, const Rml::Core::Vector2i& source_dimensions) override;
	virtual void ReleaseTexture(Rml::Core::TextureHandle texture);

	virtual void EnableScissorRegion(bool enable) override;
	virtual void SetScissorRegion(int x, int y, int width, int height) override;

	virtual void SetTransform(const Rml::Core::Matrix4f* transform) override;
};
