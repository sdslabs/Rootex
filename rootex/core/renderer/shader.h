#pragma once

#include "buffer_format.h"
#include "common/common.h"
#include "constant_buffer.h"
#include "rendering_device.h"
#include "texture.h"

class Shader
{
public:
	enum class Type
	{
		Vertex,
		Pixel
	};

protected:
	LPCWSTR m_VertexPath;
	LPCWSTR m_PixelPath;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;

	Shader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat);

	friend class ShaderLibrary;

public:
	virtual ~Shader();

	virtual void bind() const;
};

class ColorShader : public Shader
{
	ColorShader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat);
	ColorShader(ColorShader&) = delete;
	~ColorShader() = default;

	friend class ShaderLibrary;
};

class BasicShader : public Shader
{
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;

public:
	BasicShader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat);
	BasicShader(BasicShader&) = delete;
	~BasicShader() = default;

	friend class ShaderLibrary;

	virtual void bind() const override;

	void set(const Texture* texture, int slot);
};

class CPUParticlesShader : public Shader
{
	CPUParticlesShader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat);
	CPUParticlesShader(CPUParticlesShader&) = delete;
	~CPUParticlesShader() = default;

	friend class ShaderLibrary;
};

class GridShader : public Shader
{
	GridShader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat);
	GridShader(ColorShader&) = delete;
	~GridShader() = default;

	friend class ShaderLibrary;
};

class SkyShader : public Shader
{
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;

public:
	SkyShader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat);
	SkyShader(SkyShader&) = delete;
	~SkyShader() = default;

	friend class ShaderLibrary;

	virtual void bind() const override;

	void setSkyTexture(const Texture3D* texture);
};
