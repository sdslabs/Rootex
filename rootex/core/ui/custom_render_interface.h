#pragma once

#include "core/renderer/vertex_data.h"
#include "core/renderer/vertex_buffer.h"
#include "core/renderer/index_buffer.h"
#include "core/renderer/shader.h"
#include "event_manager.h"

#undef interface
#include "RmlUi/Core.h"
#define interface __STRUCT__

class CustomRenderInterface : public Rml::RenderInterface
{
	struct GeometryData
	{
		VertexBuffer vertexBuffer;
		IndexBuffer indexBuffer;
		Rml::TextureHandle textureHandle;

		GeometryData(const UIVertexData* vertices, size_t verticesSize, int* indices, size_t indicesSize, Rml::TextureHandle texture);
	};

	static unsigned int s_TextureCount;

	Ptr<Shader> m_Shader;
	HashMap<unsigned int, Ref<Texture>> m_Textures;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_ModelMatrixBuffer;
	Matrix m_UITransform;
	int m_Width;
	int m_Height;

	void render(GeometryData* geometry, const Rml::Vector2f& translation);

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
