#include "shader.h"

#include "texture.h"

#include "shaders/register_locations_pixel_shader.h"

Shader::Shader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat)
    : m_VertexPath(vertexPath)
    , m_PixelPath(pixelPath)
{
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob = RenderingDevice::GetSingleton()->createBlob(vertexPath);
	if (!vertexShaderBlob)
	{
		ERR("Vertex Shader not found");
	}
	m_VertexShader = RenderingDevice::GetSingleton()->createVS(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob = RenderingDevice::GetSingleton()->createBlob(pixelPath);
	if (!pixelShaderBlob)
	{
		ERR("Pixel Shader not found");
	}
	m_PixelShader = RenderingDevice::GetSingleton()->createPS(pixelShaderBlob.Get());

	const Vector<VertexBufferElement>& elements = vertexBufferFormat.getElements();

	Vector<D3D11_INPUT_ELEMENT_DESC> vertexDescArray;
	unsigned int offset = 0;
	for (auto& element : elements)
	{
		if (element.m_ResetOffset)
		{
			offset = 0;
		}

		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName = element.m_Name;
		desc.SemanticIndex = 0;
		desc.Format = (DXGI_FORMAT)element.m_Type;
		desc.InputSlot = element.m_Slot;
		desc.AlignedByteOffset = offset;
		desc.InputSlotClass = element.m_Class;
		desc.InstanceDataStepRate = element.m_RendersPerInstance;
		offset += VertexBufferElement::GetSize(element.m_Type);

		vertexDescArray.push_back(desc);
	}

	m_InputLayout = RenderingDevice::GetSingleton()->createVL(
	    vertexShaderBlob.Get(),
	    vertexDescArray.data(),
	    vertexDescArray.size());
}

Shader::~Shader()
{
}

void Shader::bind() const
{
	RenderingDevice::GetSingleton()->bind(m_VertexShader.Get());
	RenderingDevice::GetSingleton()->bind(m_PixelShader.Get());
	RenderingDevice::GetSingleton()->bind(m_InputLayout.Get());
}

ColorShader::ColorShader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat)
    : Shader(vertexPath, pixelPath, vertexBufferFormat)
{
}

BasicShader::BasicShader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat)
    : Shader(vertexPath, pixelPath, vertexBufferFormat)
{
	m_SamplerState = RenderingDevice::GetSingleton()->createSS();
}

void BasicShader::set(const Texture* texture, int slot)
{
	RenderingDevice::GetSingleton()->setInPixelShader(slot, 1, texture->getTextureResourceView());
}

void BasicShader::bind() const
{
	Shader::bind();
	RenderingDevice::GetSingleton()->setInPixelShader(m_SamplerState.Get());
}

ParticlesShader::ParticlesShader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat)
    : Shader(vertexPath, pixelPath, vertexBufferFormat)
{
	m_SamplerState = RenderingDevice::GetSingleton()->createSS();
}

void ParticlesShader::bind() const
{
	Shader::bind();
	RenderingDevice::GetSingleton()->setInPixelShader(m_SamplerState.Get());
}

void ParticlesShader::set(const Texture* texture, int slot)
{
	RenderingDevice::GetSingleton()->setInPixelShader(slot, 1, texture->getTextureResourceView());
}

GridShader::GridShader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat)
    : Shader(vertexPath, pixelPath, vertexBufferFormat)
{
}

SkyShader::SkyShader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat)
    : Shader(vertexPath, pixelPath, vertexBufferFormat)
{
	m_SamplerState = RenderingDevice::GetSingleton()->createSS();
}

void SkyShader::bind() const
{
	Shader::bind();
	RenderingDevice::GetSingleton()->setInPixelShader(m_SamplerState.Get());
}

void SkyShader::setSkyTexture(const TextureCube* texture)
{
	RenderingDevice::GetSingleton()->setInPixelShader(SKY_PS_CPP, 1, texture->getTextureResourceView());
}
