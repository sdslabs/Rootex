#pragma once

#include <d3d11.h>

#include "common/common.h"

/// Encapsulates Index Buffer data, to be supplied to the Input Assembler
class IndexBuffer
{
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;
	unsigned int m_Count;
	DXGI_FORMAT m_Format;

public:
	IndexBuffer(const Vector<unsigned short>& indices);
	IndexBuffer(const Vector<unsigned int>& indices);
	IndexBuffer(const int* indices, size_t size);
	~IndexBuffer() = default;

	void setData(const Vector<unsigned short>& indices);
	void setData(const Vector<unsigned int>& indices);

	void bind() const;
	unsigned int getCount() const;
	ID3D11Buffer* getBuffer() { return m_IndexBuffer.Get(); }
};
