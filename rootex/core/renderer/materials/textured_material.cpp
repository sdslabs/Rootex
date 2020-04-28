#include "textured_material.h"

#include "renderer/shader_library.h"
#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"
#include "renderer/texture.h"

TexturedMaterial::TexturedMaterial(Ref<Texture> diffuseTexture)
    : Material(ShaderLibrary::GetDiffuseShader())
    , m_DiffuseTexture(diffuseTexture)
    , m_DiffuseShader(reinterpret_cast<DiffuseShader*>(m_Shader))
{
	m_SamplerState = RenderingDevice::GetSingleton()->createSamplerState();
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);
}

void TexturedMaterial::setPSConstantBuffer(const PSDiffuseConstantBufferLights& constantBuffer)
{
	Material::setPSConstantBuffer<PSDiffuseConstantBufferLights>(constantBuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Lights], 3u);
}

void TexturedMaterial::setPSConstantBuffer(const PSDiffuseConstantBufferMaterial& constantBuffer)
{
	Material::setPSConstantBuffer<PSDiffuseConstantBufferMaterial>(constantBuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Material], 4u);
}

void TexturedMaterial::setVSConstantBuffer(const VSDiffuseConstantBuffer& constantBuffer)
{
	Material::setVSConstantBuffer<VSDiffuseConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Model], 1u);
}

void TexturedMaterial::bind()
{
	Material::bind();
	setVSConstantBuffer(VSDiffuseConstantBuffer(RenderSystem::GetSingleton()->getTopMatrix()));
	m_DiffuseShader->set(m_DiffuseTexture.get());
	setPSConstantBuffer({ LightSystem::GetSingleton()->getLights() });
	setPSConstantBuffer(PSDiffuseConstantBufferMaterial({ 0.6f, 30.0f, { 0.0f, 0.0f } }));
}
