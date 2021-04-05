#pragma once

#include "vertex_buffer.h"
#include "index_buffer.h"
#include "core/resource_files/material_resource_file.h"
#include "rendering_device.h"
#include "shader.h"
#include "viewport.h"

/// Makes the rendering draw call and set viewport, instrumental in seperating Game and HUD rendering
class Renderer
{
	const Shader* m_CurrentShader;

public:
	Renderer();
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	virtual ~Renderer() = default;

	void setViewport(Viewport& viewport);

	void resetCurrentShader();
	void bind(MaterialResourceFile* material);
	void draw(const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer) const;
	void drawInstanced(const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer, const VertexBuffer* instanceBuffer, unsigned int instances) const;
};
