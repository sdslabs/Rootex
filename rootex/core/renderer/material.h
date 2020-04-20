#pragma once

#include "constant_buffer.h"
#include "shader.h"

class Texture;

class Material
{
public:
	enum class VertexConstantBufferType
	{
		Model,
		ModelInverse,
		End
	};

protected:
	
	Shader* m_Shader;
	Vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_PSConstantBuffer;
	Vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_VSConstantBuffer;

	Material(Shader* shader);

public:
	Material();
	virtual ~Material() = default;

	virtual void bind() = 0;

	virtual void setVertexShaderConstantBuffer(const VertexConstantBufferType type, const Matrix& constantBuffer);
};

class ColorMaterial : public Material
{
public:
	enum class PixelConstantBufferType
	{
		Color,
		End
	};
	
	void setPixelShaderConstantBuffer(const PSSolidConstantBuffer& constantBuffer);

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

	enum class PixelConstantBufferType
	{
		Lights,
		Material,
		End
	};
	TexturedMaterial(Ref<Texture> diffuseTexture);
	~TexturedMaterial() = default;
	void setPixelShaderConstantBuffer(const PSDiffuseConstantBuffer& constantBuffer);
	
	void bind() override;
};

class CPUParticlesMaterial : public Material
{
public:

	enum class PixelConstantBufferType
	{
		Color,
		End
	};
	CPUParticlesMaterial();
	~CPUParticlesMaterial() = default;
	void setPixelShaderConstantBuffer(const PSSolidConstantBuffer& constantBuffer);

	void bind() override;
};
