#include "material.h"

#include "shader_library.h"
#include "texture.h"

#include "framework/systems/light_system.h"

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
	const PSDiffuseConstantBuffer Cb = { LightSystem::GetSingleton()->getLights(), { 0.6f, 30.0f, { 0.0f, 0.0f } } };
	setShaderConstantBuffer(Cb);
    Material::bind();
}

CPUParticlesMaterial::CPUParticlesMaterial()
    : Material(ShaderLibrary::GetCPUParticlesShader())
{
}

void CPUParticlesMaterial::bind() const
{
	Material::bind();
}
