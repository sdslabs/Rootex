#pragma once

#include "vertex_buffer.h"
#include "vertex_buffer_format.h"

class VertexArray
{
protected:
public:
	VertexArray() {}
	virtual ~VertexArray() {}

	virtual void addBuffer(const VertexBuffer& vertexBuffer, const VertexBufferFormat& vertexBufferFormat) = 0;
	virtual void bind() const {}
	virtual void unbind() const {}
};
