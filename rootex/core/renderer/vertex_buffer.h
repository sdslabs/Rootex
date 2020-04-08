#pragma once

#include <d3d11.h>

#include "common/common.h"
#include "renderer/vertex_data.h"

/// Encapsulates a vector of vertices to be used as Vertex Buffer
class VertexBuffer
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	Vector<VertexData> m_Buffer;

public:
	VertexBuffer(const Vector<VertexData>& buffer);
	~VertexBuffer();

	void bind() const;
};
