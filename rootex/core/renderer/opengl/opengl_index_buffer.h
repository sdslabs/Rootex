#pragma once

#include "renderer/index_buffer.h"

class OpenGLIndexBuffer : public IndexBuffer
{
	int m_BufferID;

protected:
	OpenGLIndexBuffer(Vector<unsigned int>& indices);
	virtual ~OpenGLIndexBuffer() = default;

	void bind() const override;
	void unbind() const override;

	unsigned int getCount() override;
};
