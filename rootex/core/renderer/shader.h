#pragma once

#include "common/common.h"

#include "buffer_format.h"
#include "constant_buffer.h"
#include "rendering_device.h"
#include "texture.h"

class Shader
{
public:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;

public:
	Shader() = default;
	Shader(const String& vertexPath, const String& pixelPath, const BufferFormat& vertexBufferFormat);
	~Shader() = default;

	void bind() const;
};
