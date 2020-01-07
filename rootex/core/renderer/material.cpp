#include "material.h"

#include "shader_library.h"
#include "texture.h"

Ref<CubeMaterial> Material::CreateDefault()
{
	return Ref<CubeMaterial>(new CubeMaterial());
}

void Material::bind() const
{
	m_Shader->bind();
}

CubeMaterial::CubeMaterial()
    : Material(ShaderLibrary::GetShader("Default"))
{
	m_ConstantBufferColors.m_Colors[0] = ColorPresets::Red;
	m_ConstantBufferColors.m_Colors[1] = ColorPresets::Green;
	m_ConstantBufferColors.m_Colors[2] = ColorPresets::Blue;
	m_ConstantBufferColors.m_Colors[3] = ColorPresets::Yellow;
	m_ConstantBufferColors.m_Colors[4] = ColorPresets::White;
	m_ConstantBufferColors.m_Colors[5] = ColorPresets::Orange;
}

void CubeMaterial::bind() const
{
	m_Shader->setConstantBuffer(m_ConstantBufferColors);
	Material::bind();
}
