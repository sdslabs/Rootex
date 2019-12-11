#pragma once

#include "renderer/vertex_buffer.h"

class OpenGLVertexBuffer : public VertexBuffer
{
	unsigned int m_BufferID;

public:
	OpenGLVertexBuffer(Vector<char> buffer);
	~OpenGLVertexBuffer();
};