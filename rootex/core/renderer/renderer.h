#pragma once

#include "vertex_array.h"
#include "index_buffer.h"
#include "shader.h"

enum class RendererType
{
	DIRECTX,
	OPENGL
};

class Renderer
{
protected:
	Renderer(RendererType type);
	~Renderer();

public:
	virtual void clear() const = 0;
	virtual void draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader) = 0;
};
