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
void Material::setPSConstantBuffer(const T& constantBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>& bufferPointer, UINT slot)
{
	if (bufferPointer == nullptr)
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

		bufferPointer = RenderingDevice::GetSingleton()->createPSConstantBuffer(&cbd, &csd, slot);
		RenderingDevice::GetSingleton()->setPSConstantBuffer(bufferPointer.Get(), slot);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE subresource;
		RenderingDevice::GetSingleton()->mapBuffer(bufferPointer.Get(), subresource);
		memcpy(subresource.pData, &constantBuffer, sizeof(constantBuffer));
		RenderingDevice::GetSingleton()->unmapBuffer(bufferPointer.Get());

		RenderingDevice::GetSingleton()->setPSConstantBuffer(bufferPointer.Get(), slot);
	}
}

template <typename T>
void Material::setVSConstantBuffer(const T& constantBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>& bufferPointer, UINT slot)
{
	if (bufferPointer == nullptr)
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

		bufferPointer = RenderingDevice::GetSingleton()->createVSConstantBuffer(&cbd, &csd, slot);
		RenderingDevice::GetSingleton()->setVSConstantBuffer(bufferPointer.Get(), slot);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE subresource;
		RenderingDevice::GetSingleton()->mapBuffer(bufferPointer.Get(), subresource);
		memcpy(subresource.pData, &constantBuffer, sizeof(constantBuffer));
		RenderingDevice::GetSingleton()->unmapBuffer(bufferPointer.Get());

		RenderingDevice::GetSingleton()->setVSConstantBuffer(bufferPointer.Get(), slot);
	}
}

ColorMaterial::ColorMaterial()
{
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);
}

void ColorMaterial::setPSConstantBuffer(const PSSolidConstantBuffer& constantBuffer)
{
	Material::setPSConstantBuffer<PSSolidConstantBuffer>(constantBuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Color], 3u);
}

void ColorMaterial::setVSConstantBuffer(const VSSolidConstantBuffer& constantBuffer)
{
	Material::setVSConstantBuffer<VSSolidConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Model], 1u);
}

void ColorMaterial::bind()
{
	m_Shader->bind();
	setVSConstantBuffer(VSSolidConstantBuffer(RenderSystem::GetSingleton()->getTopMatrix()));
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
	m_Shader->bind();
	setVSConstantBuffer(VSDiffuseConstantBuffer(RenderSystem::GetSingleton()->getTopMatrix()));
	m_DiffuseShader->set(m_DiffuseTexture.get());
	setPSConstantBuffer({ LightSystem::GetSingleton()->getLights() });
	setPSConstantBuffer(PSDiffuseConstantBufferMaterial({ 0.6f, 30.0f, { 0.0f, 0.0f } }));
}

CPUParticlesMaterial::CPUParticlesMaterial()
    : Material(ShaderLibrary::GetCPUParticlesShader())
{
	m_PSConstantBuffer.resize((int)PixelConstantBufferType::End, nullptr);
	m_VSConstantBuffer.resize((int)VertexConstantBufferType::End, nullptr);
}

void CPUParticlesMaterial::setPSConstantBuffer(const PSSolidConstantBuffer& constantBuffer)
{
	Material::setPSConstantBuffer<PSSolidConstantBuffer>(constantBuffer, m_PSConstantBuffer[(int)PixelConstantBufferType::Color], 3u);
}

void CPUParticlesMaterial::setVSConstantBuffer(const VSSolidConstantBuffer& constantBuffer)
{
	Material::setVSConstantBuffer<VSSolidConstantBuffer>(constantBuffer, m_VSConstantBuffer[(int)VertexConstantBufferType::Model], 1u);
}

void CPUParticlesMaterial::bind()
{
	m_Shader->bind();
	setVSConstantBuffer(VSSolidConstantBuffer({ RenderSystem::GetSingleton()->getTopMatrix() }));
}
