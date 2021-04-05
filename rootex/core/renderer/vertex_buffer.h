#pragma once

#include <d3d11.h>

#include "common/common.h"
#include "renderer/vertex_data.h"

/// Encapsulates a vector of vertices to be used as Vertex Buffer
class VertexBuffer
{
	/// Pointer given by DirectX API to refer the vertex buffer created on GPU
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	unsigned int m_Stride;
	unsigned int m_Count;

public:
	VertexBuffer(const char* buffer, unsigned int elementCount, unsigned int stride, D3D11_USAGE usage, int cpuAccess);
	~VertexBuffer() = default;

	void bind() const;

	unsigned int getCount() const { return m_Count; }
	unsigned int getStride() const { return m_Stride; }
	ID3D11Buffer* getBuffer() const { return m_VertexBuffer.Get(); };
};
