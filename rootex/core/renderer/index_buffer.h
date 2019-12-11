#pragma once

#include "common/common.h"

class IndexBuffer
{
protected:
	IndexBuffer(Vector<unsigned int> indices) {}
	virtual ~IndexBuffer() = default;

	virtual void bind() const = 0;
	virtual void unbind() const = 0;

	virtual unsigned int getCount() = 0;
};
