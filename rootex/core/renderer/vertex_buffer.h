#pragma once

#include "common/common.h"
#include "renderer/vertex_data.h"

class VertexBuffer
{
	Vector<VertexData> m_Buffer;

public:
	VertexBuffer(const Vector<VertexData>& buffer);
	~VertexBuffer() = default;
};
