#pragma once

#include "constant_buffer.h"
#include "shader.h"

class Texture;
class CubeMaterial;

class Material
{
protected:
	Shader* m_Shader;

public:
	static Ref<CubeMaterial> CreateDefault();

	Material(Shader* shader)
	    : m_Shader(shader)
	{
	}
	virtual ~Material() = default;

	virtual void bind() const;

	void bindShaderTexture(const Texture* texture) const { m_Shader->bindTexture(texture); }
	void setShaderMatrix(Shader::ConstantBufferType matrixType, const Matrix& matrix) { m_Shader->setConstantBuffer(matrixType, matrix); }
};

class CubeMaterial : public Material
{
	PSConstantBuffer m_ConstantBufferColors;

public:
	CubeMaterial();
	~CubeMaterial() = default;

	void bind() const override;
};
