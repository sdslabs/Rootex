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

class ParticlesShader : public Shader
{
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;

public:
	ParticlesShader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat);
	ParticlesShader(ParticlesShader&) = delete;
	~ParticlesShader() = default;

	friend class ShaderLibrary;

	virtual void bind() const override;

	void set(const Texture* texture, int slot);
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

	void setSkyTexture(const TextureCube* texture);
};

class AnimationShader : public Shader
{
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;

public:
	AnimationShader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat);
	AnimationShader(AnimationShader&) = delete;
	~AnimationShader() = default;

	friend class ShaderLibrary;

	virtual void bind() const override;

	void set(const Texture* texture, int slot);
};

class FXAAShader : public Shader
{
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;

public:
	FXAAShader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat);
	FXAAShader(FXAAShader&) = delete;
	~FXAAShader() = default;

	friend class ShaderLibrary;

	virtual void bind() const override;

	void set(ID3D11ShaderResourceView* srv);
};

class LumaShader : public Shader
{
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;

public:
	LumaShader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat);
	LumaShader(LumaShader&) = delete;
	~LumaShader() = default;

	friend class ShaderLibrary;

	virtual void bind() const override;

	void set(ID3D11ShaderResourceView* srv);
};
