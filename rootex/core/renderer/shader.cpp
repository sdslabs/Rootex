#include "shader.h"

#include "utils.h"

Shader::Shader(const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat)
    : m_VertexPath(vertexPath)
    , m_PixelPath(pixelPath)
{
	ID3DBlob* vertexShaderBlob = RenderingDevice::GetSingleton()->createBlob(vertexPath);
	m_VertexShader = RenderingDevice::GetSingleton()->initVertexShader(vertexShaderBlob);
	
	ID3DBlob* pixelShaderBlob = RenderingDevice::GetSingleton()->createBlob(pixelPath);
	m_PixelShader = RenderingDevice::GetSingleton()->initPixelShader(pixelShaderBlob);

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
	    vertexShaderBlob, 
		vertexDescArray.data(), 
		vertexDescArray.size());

	SafeRelease(&vertexShaderBlob);
	SafeRelease(&pixelShaderBlob);
}

Shader::~Shader()
{
	SafeRelease(&m_VertexShader);
	SafeRelease(&m_PixelShader);
}

void Shader::bind() const
{
	RenderingDevice::GetSingleton()->bind(m_VertexShader);
	RenderingDevice::GetSingleton()->bind(m_PixelShader);
}

void Shader::unbind() const
{
	RenderingDevice::GetSingleton()->unbindShaderResources();
}

void Shader::setTransformConstantBuffer(const VSTransformBuffer& transformBuffer)
{
	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(transformBuffer);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = { 0 };
	csd.pSysMem = &transformBuffer;

	RenderingDevice::GetSingleton()->initVSTransformConstantBuffer(&cbd, &csd);
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
