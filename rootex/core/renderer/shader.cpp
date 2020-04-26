#include "shader.h"

#include "texture.h"

Shader::Shader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat)
    : m_VertexPath(vertexPath)
    , m_PixelPath(pixelPath)
{
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob = RenderingDevice::GetSingleton()->createBlob(vertexPath);
	if (!vertexShaderBlob)
	{
		ERR("Vertex Shader not found");
	}
	m_VertexShader = RenderingDevice::GetSingleton()->createVertexShader(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob = RenderingDevice::GetSingleton()->createBlob(pixelPath);
	if (!pixelShaderBlob)
	{
		ERR("Pixel Shader not found");
	}
	m_PixelShader = RenderingDevice::GetSingleton()->createPixelShader(pixelShaderBlob.Get());

	const Vector<VertexBufferElement>& elements = vertexBufferFormat.getElements();

	Vector<D3D11_INPUT_ELEMENT_DESC> vertexDescArray;
	unsigned int offset = 0;
	for (auto& element : elements)
	{
		D3D11_INPUT_ELEMENT_DESC desc;
		desc = { element.m_Name, 0, (DXGI_FORMAT)element.m_Type, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		offset += VertexBufferElement::GetSize(element.m_Type);

		vertexDescArray.push_back(desc);
	}

	m_InputLayout = RenderingDevice::GetSingleton()->createVertexLayout(
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

DiffuseShader::DiffuseShader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat)
    : Shader(vertexPath, pixelPath, vertexBufferFormat)
{
	m_SamplerState = RenderingDevice::GetSingleton()->createSamplerState();
}

void DiffuseShader::set(const Texture* texture)
{
	RenderingDevice::GetSingleton()->setInPixelShader(0, 1, texture->getTextureResourceView());
}

void DiffuseShader::bind() const
{
	Shader::bind();
	RenderingDevice::GetSingleton()->setInPixelShader(m_SamplerState.Get());
}

CPUParticlesShader::CPUParticlesShader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat)
    : Shader(vertexPath, pixelPath, vertexBufferFormat)
{
}

GridShader::GridShader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat)
    : Shader(vertexPath, pixelPath, vertexBufferFormat)
{
}
