#pragma once

#include <d3d11.h>

#include "common/common.h"

/// Encapsulates Index Buffer data, to be supplied to the Input Assembler
class IndexBuffer
{
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;
	Vector<unsigned short> m_Buffer;

public:
	IndexBuffer(const Vector<unsigned short>& indices);
	~IndexBuffer();

	void bind() const;
	unsigned int getCount() const;
};
