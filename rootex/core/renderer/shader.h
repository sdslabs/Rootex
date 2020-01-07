#pragma once

#include "buffer_format.h"
#include "common/common.h"
#include "constant_buffer.h"
#include "rendering_device.h"
#include "texture.h"

class Shader
{
protected:
	LPCWSTR m_VertexPath;
	LPCWSTR m_PixelPath;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
	
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;

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

	void bindTexture(const Texture* texture);
	void setConstantBuffer(const ConstantBufferType& type, const Matrix& constantBuffer);
	void setConstantBuffer(const PSConstantBuffer& constantBuffer);
};
