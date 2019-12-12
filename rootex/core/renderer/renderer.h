#pragma once

#include "index_buffer.h"
#include "shader.h"
#include "vertex_array.h"

class Renderer
{
protected:
	Renderer() {}
	virtual ~Renderer() {}

public:
	virtual void clear() const = 0;
	virtual void draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader) = 0;
};
