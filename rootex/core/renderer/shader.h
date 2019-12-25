#pragma once

#include "buffer_format.h"
#include "common/common.h"
#include "constant_buffer.h"
#include "rendering_device.h"

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

	enum class ConstantBufferType
	{
		Model,
		View,
		Projection
	};

	Shader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormatconst);
	~Shader();

	void bind() const;
	void unbind() const;

	void setConstantBuffer(const ConstantBufferType& type, const DirectX::XMMATRIX& constantBuffer);
	void setConstantBuffer(const PSConstantBuffer& constantBuffer);
};
