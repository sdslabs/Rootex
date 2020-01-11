#include "material.h"

#include "shader_library.h"
#include "texture.h"

Material::Material()
    : m_Shader(ShaderLibrary::GetDefaultShader())
{
}

Material::Material(Shader* shader)
    : m_Shader(shader)
{
}

void Material::bind() const
{
	m_Shader->bind();
}

DiffuseMaterial::DiffuseMaterial(Ref<Texture> diffuseTexture)
    : Material(ShaderLibrary::GetDiffuseShader())
    , m_DiffuseTexture(diffuseTexture)
    , m_DiffuseShader(reinterpret_cast<DiffuseShader*>(m_Shader))
{
	m_SamplerState = RenderingDevice::GetSingleton()->createSamplerState();
}

void DiffuseMaterial::bind() const
{
	m_DiffuseShader->set(m_DiffuseTexture.get());
    Material::bind();
}
