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

	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;

public:
	enum class Type
	{
		Vertex,
		Pixel
	};

	Shader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormatconst);
	~Shader();

	void bind() const;
	void unbind() const;
	
	void setTransformConstantBuffer(const VSTransformBuffer& transformBuffer);
	void setConstantBuffer(const VSConstantBuffer& constantBuffer);
	void setConstantBuffer(const PSConstantBuffer& constantBuffer);
};
