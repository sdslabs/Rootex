#pragma once

#include "constant_buffer.h"
#include "shader.h"

class Texture;

class Material
{
protected:
	Shader* m_Shader;
	Vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_PSConstantBuffer;
	Vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_VSConstantBuffer;

	Material(Shader* shader);

	template <typename T>
	void setPixelShaderConstantBuffer(const T& constantBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>& pointer, UINT slot);
	template <typename T>
	void setVertexShaderConstantBuffer(const T& constantBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>& pointer, UINT slot);

public:
	Material();
	virtual ~Material() = default;
	virtual void bind() = 0;
};

class ColorMaterial : public Material
{
public:
	enum class VertexConstantBufferType
	{
		Model,
		End
	};
	enum class PixelConstantBufferType
	{
		Color,
		End
	};

	void setPixelShaderConstantBuffer(const PSSolidConstantBuffer& constantBuffer);
	void setVertexShaderConstantBuffer(const VSSolidConstantBuffer& constantBuffer);

	void bind() override;

	ColorMaterial();
	~ColorMaterial() = default;
};

class TexturedMaterial : public Material
{
	DiffuseShader* m_DiffuseShader;
	Ref<Texture> m_DiffuseTexture;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;

public:
	enum class VertexConstantBufferType
	{
		Model,
		End
	};
	enum class PixelConstantBufferType
	{
		Lights,
		Material,
		End
	};
	TexturedMaterial(Ref<Texture> diffuseTexture);
	~TexturedMaterial() = default;

	void setPixelShaderConstantBuffer(const PSDiffuseConstantBufferLights& constantBuffer);
	void setPixelShaderConstantBuffer(const PSDiffuseConstantBufferMaterial& constantBuffer);
	void setVertexShaderConstantBuffer(const VSDiffuseConstantBuffer& constantBuffer);

	void bind() override;
};

class CPUParticlesMaterial : public Material
{
public:
	enum class VertexConstantBufferType
	{
		Model,
		End
	};
	enum class PixelConstantBufferType
	{
		Color,
		End
	};
	CPUParticlesMaterial();
	~CPUParticlesMaterial() = default;
	void setPixelShaderConstantBuffer(const PSSolidConstantBuffer& constantBuffer);
	void setVertexShaderConstantBuffer(const VSSolidConstantBuffer& constantBuffer);

	void bind() override;
};
