#include "opengl_index_buffer.h"

OpenGLIndexBuffer::OpenGLIndexBuffer(Vector<unsigned int>& indices)
    : IndexBuffer(indices)
{
}

void OpenGLIndexBuffer::bind() const
{
}

void OpenGLIndexBuffer::unbind() const
{
}

unsigned int OpenGLIndexBuffer::getCount()
{
	return 0;
}
