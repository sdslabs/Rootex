#include "shader.h"

#include "texture.h"

#include "shaders/register_locations_pixel_shader.h"

Shader::Shader(const String& vertexPath, const String& pixelPath, const BufferFormat& vertexBufferFormat)
{
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob = RenderingDevice::GetSingleton()->compileShader(vertexPath, "main", "vs_4_0");
	if (!vertexShaderBlob)
	{
		ERR("Could not compile vertex shader: " + vertexPath);
		m_IsValid = false;
		return;
	}
	m_VertexShader = RenderingDevice::GetSingleton()->createVS(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob = RenderingDevice::GetSingleton()->compileShader(pixelPath, "main", "ps_4_0");
	if (!pixelShaderBlob)
	{
		ERR("Could not compile pixel shader: " + pixelPath);
		m_IsValid = false;
		return;
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

	m_IsValid = true;
}

void Shader::bind() const
{
	RenderingDevice::GetSingleton()->bind(m_VertexShader.Get());
	RenderingDevice::GetSingleton()->bind(m_PixelShader.Get());
	RenderingDevice::GetSingleton()->bind(m_InputLayout.Get());
}
