#pragma once

#include "common/common.h"

class VertexBuffer
{
protected:
	VertexBuffer(const Vector<char>& buffer) {}
	virtual ~VertexBuffer() = default;

public:
	virtual void bind() const = 0;
	virtual void unbind() const = 0;
};
