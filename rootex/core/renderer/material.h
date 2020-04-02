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
		View,
		Projection,
		END
	};

protected:
	

	Shader* m_Shader;
	//Vector<ID3D11Buffer*> m_PSConstantBuffer;
	ID3D11Buffer* m_PSConstantBuffer1;
	ID3D11Buffer* m_PSConstantBuffer2;
	Vector<ID3D11Buffer*> m_VSConstantBuffer;

	Material(Shader* shader);

public:
	Material();
	virtual ~Material() = default;

	virtual void bind() = 0;

	virtual void setVertexShaderConstantBuffer(const VertexConstantBufferType type, const Matrix& constantBuffer);

	//void setShaderConstantBuffer(Shader::VertexConstantBufferType matrixType, const Matrix& matrix) { m_Shader->set(matrixType, matrix); }
	//void setShaderConstantBuffer(PSSolidConstantBuffer& Cb) { m_Shader->set(Cb); }
};

class ColorMaterial : public Material
{
public:
	
	void setPixelShaderConstantBuffer (const PSSolidConstantBuffer& constantBuffer);

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
<<<<<<< HEAD
	TexturedMaterial(Ref<Texture> diffuseTexture);
	~TexturedMaterial() = default;
	void setShaderConstantBuffer(const PSDiffuseConstantBuffer& Cb) const { m_Shader->set(Cb); }
=======
	DiffuseMaterial(Ref<Texture> diffuseTexture);
	~DiffuseMaterial() = default;
	void setPixelShaderConstantBuffer(const PSDiffuseConstantBuffer& constantBuffer);
>>>>>>> wip refactor material and shader


	void bind() override;
};

class CPUParticlesMaterial : public Material
{
public:
	CPUParticlesMaterial();
	~CPUParticlesMaterial() = default;
	void setPixelShaderConstantBuffer(const PSDiffuseConstantBuffer& constantBuffer);

	void bind() override;
};
