#pragma once

#include "common/common.h"
#include "rendering_device.h"
#include "buffer_format.h"
#include "constant_buffer.h"

class Shader
{
protected:
	LPCWSTR m_VertexPath;
	LPCWSTR m_PixelPath;

	ID3DBlob* m_VertexShaderBlob;
	ID3DBlob* m_PixelShaderBlob;

public:
	enum class Type
	{
		Vertex,
		Pixel
	};

	Shader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath);
	~Shader() = default;

	void setVertexBufferFormat(const BufferFormat& vertexBufferFormat);
	void setConstantBuffer(const VSConstantBuffer& constantBuffer);
	void setConstantBuffer(const PSConstantBuffer& constantBuffer);
};
