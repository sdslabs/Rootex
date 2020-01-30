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

	void setShaderConstantBuffer(Shader::ConstantBufferType matrixType, const Matrix& matrix) { m_Shader->set(matrixType, matrix); }
	void setShaderConstantBuffer(PSConstantBuffer& Cb) { m_Shader->set(Cb); }
};

class DiffuseMaterial : public Material
{
	DiffuseShader* m_DiffuseShader;
	Ref<Texture> m_DiffuseTexture;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;

public:
	DiffuseMaterial(Ref<Texture> diffuseTexture);
	~DiffuseMaterial() = default;

	void bind() const override;
};
