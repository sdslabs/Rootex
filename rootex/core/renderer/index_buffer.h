#pragma once

#include "common/common.h"

class IndexBuffer
{
	Vector<unsigned int>& m_Indices;

protected:
	IndexBuffer(Vector<unsigned int>& indices)
	    : m_Indices(indices)
	{
	}

	virtual ~IndexBuffer() = default;

	virtual void bind() const = 0;
	virtual void unbind() const = 0;

	virtual unsigned int getCount() = 0;
};
