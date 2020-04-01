#pragma once

#include "constant_buffer.h"
#include "shader.h"

class Texture;

class Material
{
protected:
	Shader* m_Shader;

	Material(Shader* shader);

public:
	Material();
	virtual ~Material() = default;

	virtual void bind() const;

	void setShaderConstantBuffer(Shader::VertexConstantBufferType matrixType, const Matrix& matrix) { m_Shader->set(matrixType, matrix); }
	void setShaderConstantBuffer(PSSolidConstantBuffer& Cb) { m_Shader->set(Cb); }
};

class TexturedMaterial : public Material
{
	DiffuseShader* m_DiffuseShader;
	Ref<Texture> m_DiffuseTexture;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;

public:
	TexturedMaterial(Ref<Texture> diffuseTexture);
	~TexturedMaterial() = default;
	void setShaderConstantBuffer(const PSDiffuseConstantBuffer& Cb) const { m_Shader->set(Cb); }

	void bind() const override;
};

class CPUParticlesMaterial : public Material
{
public:
	CPUParticlesMaterial();
	~CPUParticlesMaterial() = default;
	
	void bind() const override;
};
