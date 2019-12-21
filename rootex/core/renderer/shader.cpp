#include "shader.h"

#include "utils.h"

Shader::Shader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath)
    : m_VertexPath(vertexPath)
    , m_PixelPath(pixelPath)
{
	m_VertexShaderBlob = RenderingDevice::GetSingleton()->createBlob(vertexPath);
	m_VertexShader = RenderingDevice::GetSingleton()->initVertexShader(m_VertexShaderBlob);
	
	m_PixelShaderBlob = RenderingDevice::GetSingleton()->createBlob(pixelPath);
	m_PixelShader = RenderingDevice::GetSingleton()->initPixelShader(m_PixelShaderBlob);
}

Shader::~Shader()
{
	SafeRelease(&m_VertexShaderBlob);
	SafeRelease(&m_VertexShader);
	SafeRelease(&m_PixelShaderBlob);
	SafeRelease(&m_PixelShader);
}

void Shader::bind() const
{
	RenderingDevice::GetSingleton()->bind(m_VertexShader);
	RenderingDevice::GetSingleton()->bind(m_PixelShader);
}

void Shader::setVertexBufferFormat(const BufferFormat& vertexBufferFormat)
{
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

	RenderingDevice::GetSingleton()->initVertexLayout(
	    m_VertexShaderBlob, 
		vertexDescArray.data(), 
		vertexDescArray.size());
}

void Shader::setConstantBuffer(const VSConstantBuffer& constantBuffer)
{
	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(constantBuffer);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = { 0 };
	csd.pSysMem = &constantBuffer;

	RenderingDevice::GetSingleton()->initVSConstantBuffer(&cbd, &csd);
}

void Shader::setConstantBuffer(const PSConstantBuffer& constantBuffer)
{
	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(constantBuffer);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = { 0 };
	csd.pSysMem = &constantBuffer;

	RenderingDevice::GetSingleton()->initPSConstantBuffer(&cbd, &csd);
}
