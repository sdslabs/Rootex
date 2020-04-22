#include "material.h"

#include "shader_library.h"
#include "texture.h"

#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"

Material::Material()
    : m_Shader(ShaderLibrary::GetDefaultShader())
{
}

Material::Material(Shader* shader)
    : m_Shader(shader)
{
}

template <typename T>
void Material::setPixelShaderConstantBuffer(const T& constantBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>& pointer, UINT slot)
{
	if (pointer == nullptr)
	{
		D3D11_BUFFER_DESC cbd = { 0 };
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(T);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = { 0 };
		csd.pSysMem = &constantBuffer;

		pointer = RenderingDevice::GetSingleton()->createPSConstantBuffer(&cbd, &csd, slot);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE subresource;
		RenderingDevice::GetSingleton()->getBufferMappedContext(pointer.Get(), subresource);
		memcpy(subresource.pData, &constantBuffer, sizeof(constantBuffer));
		RenderingDevice::GetSingleton()->unmapBuffer(pointer.Get());
	}
}

template <typename T>
void Material::setVertexShaderConstantBuffer(const T& constantBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>& pointer, UINT slot)
{
	if (pointer == nullptr)
	{
		D3D11_BUFFER_DESC cbd = { 0 };
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(T);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = { 0 };
		csd.pSysMem = &constantBuffer;

		pointer = RenderingDevice::GetSingleton()->createVSConstantBuffer(&cbd, &csd, slot);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE subresource;
		RenderingDevice::GetSingleton()->getBufferMappedContext(pointer.Get(), subresource);
		memcpy(subresource.pData, &constantBuffer, sizeof(constantBuffer));
		RenderingDevice::GetSingleton()->unmapBuffer(pointer.Get());
	}
}

TexturedMaterial::TexturedMaterial(Ref<Texture> diffuseTexture)
    : Material(ShaderLibrary::GetDiffuseShader())
    , m_DiffuseTexture(diffuseTexture)
    , m_DiffuseShader(reinterpret_cast<DiffuseShader*>(m_Shader))
{
	m_SamplerState = RenderingDevice::GetSingleton()->createSamplerState();
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);
}

void ColorMaterial::setPixelShaderConstantBuffer(const PSSolidConstantBuffer& constantBuffer)
{
	Material::setPixelShaderConstantBuffer<PSSolidConstantBuffer>(constantBuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Color], 3u);
}

void ColorMaterial::setVertexShaderConstantBuffer(const VSSolidConstantBuffer& constantBuffer)
{
	Material::setVertexShaderConstantBuffer<VSSolidConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Model], 1u);
}

void TexturedMaterial::setPixelShaderConstantBuffer(const PSDiffuseConstantBufferLights& constantBuffer)
{
	Material::setPixelShaderConstantBuffer<PSDiffuseConstantBufferLights>(constantBuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Lights], 3u);
}

void TexturedMaterial::setPixelShaderConstantBuffer(const PSDiffuseConstantBufferMaterial& constantBuffer)
{
	Material::setPixelShaderConstantBuffer<PSDiffuseConstantBufferMaterial>(constantBuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Material], 4u);
}

void TexturedMaterial::setVertexShaderConstantBuffer(const VSDiffuseConstantBuffer& constantBuffer)
{
	Material::setVertexShaderConstantBuffer<VSDiffuseConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Model], 1u);
}

void CPUParticlesMaterial::setPixelShaderConstantBuffer(const PSSolidConstantBuffer& constantBuffer)
{
	Material::setPixelShaderConstantBuffer<PSSolidConstantBuffer>(constantBuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Color], 3u);
}

void CPUParticlesMaterial::setVertexShaderConstantBuffer(const VSSolidConstantBuffer& constantBuffer)
{
	Material::setVertexShaderConstantBuffer<VSSolidConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Model], 1u);
}

void ColorMaterial::bind()
{
	setVertexShaderConstantBuffer(VSSolidConstantBuffer(RenderSystem::GetSingleton()->getTopMatrix()));
	m_Shader->bind();
}

void TexturedMaterial::bind()
{
	setVertexShaderConstantBuffer(VSDiffuseConstantBuffer(RenderSystem::GetSingleton()->getTopMatrix()));
	m_Shader->bind();
	m_DiffuseShader->set(m_DiffuseTexture.get());
	setPixelShaderConstantBuffer({ LightSystem::GetSingleton()->getLights() });
	setPixelShaderConstantBuffer(PSDiffuseConstantBufferMaterial ({ 0.6f, 30.0f, { 0.0f, 0.0f } }));
}

CPUParticlesMaterial::CPUParticlesMaterial()
    : Material(ShaderLibrary::GetCPUParticlesShader())
{
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);
}

void CPUParticlesMaterial::bind()
{
	setVertexShaderConstantBuffer(VSSolidConstantBuffer ({ RenderSystem::GetSingleton()->getTopMatrix() }));
	m_Shader->bind();
}

ColorMaterial::ColorMaterial()
    : Material()
{
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);
}
