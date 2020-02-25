#pragma once

#include "vertex_buffer.h"
#include "index_buffer.h"
#include "material.h"
#include "rendering_device.h"
#include "viewport.h"

class Renderer
{
public:
	Renderer();
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	virtual ~Renderer() = default;

	void setViewport(Viewport& viewport);

	void draw(const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer, const Material* material) const;
};
