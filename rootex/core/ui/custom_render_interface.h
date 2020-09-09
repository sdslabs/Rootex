#pragma once

#include "core/renderer/material_library.h"
#include "event_manager.h"

#undef interface
#include "RmlUi/Core.h"
#define interface __STRUCT__

class CustomRenderInterface : public Rml::Core::RenderInterface
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

	virtual void RenderGeometry(Rml::Core::Vertex* vertices, int numVertices, int* indices, int numIndices, Rml::Core::TextureHandle texture, const Rml::Core::Vector2f& translation) override;
	
	virtual Rml::Core::CompiledGeometryHandle CompileGeometry(Rml::Core::Vertex* vertices, int numVertices, int* indices, int numIndices, Rml::Core::TextureHandle texture) override;
	virtual void RenderCompiledGeometry(Rml::Core::CompiledGeometryHandle geometry, const Rml::Core::Vector2f& translation) override;
	virtual void ReleaseCompiledGeometry(Rml::Core::CompiledGeometryHandle geometry) override;

	virtual bool LoadTexture(Rml::Core::TextureHandle& textureHandle, Rml::Core::Vector2i& textureDimensions, const String& source) override;
	virtual bool GenerateTexture(Rml::Core::TextureHandle& textureHandle, const byte* source, const Rml::Core::Vector2i& sourceDimensions) override;
	virtual void ReleaseTexture(Rml::Core::TextureHandle texture);

	virtual void EnableScissorRegion(bool enable) override;
	virtual void SetScissorRegion(int x, int y, int width, int height) override;

	virtual void SetTransform(const Rml::Core::Matrix4f* transform) override;
};
