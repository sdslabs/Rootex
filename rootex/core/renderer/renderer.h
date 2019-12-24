#pragma once

#include "vertex_buffer.h"
#include "index_buffer.h"
#include "shader.h"
#include "rendering_device.h"
#include "viewport.h"

class Renderer
{
protected:
	unsigned int m_Height;
	unsigned int m_Width;
	const DirectX::XMMATRIX* m_ImmediateTransform;

public:
	Renderer(unsigned int w = 640, unsigned int h = 480);
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	virtual ~Renderer() = default;

	void setViewport(Viewport& viewport);

	void setImmediateTransform(const DirectX::XMMATRIX* transformMatrix);
	void clear() const;
	void draw(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer, const Shader& shader) const;
};
