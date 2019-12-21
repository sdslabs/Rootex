#pragma once

#include "common/common.h"

class IndexBuffer
{
protected:
	Vector<unsigned short> m_Buffer;

public:
	IndexBuffer(const Vector<unsigned short>& indices);
	~IndexBuffer() = default;

	unsigned int getCount() const;
};
