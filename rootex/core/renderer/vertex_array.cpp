#include "vertex_array.h"

#include <d3d11.h>

#include "rendering_device.h"

void VertexArray::addBuffer(const VertexBuffer& vertexBuffer, const VertexBufferFormat& vertexBufferFormat)
    :m_V
{
	const Vector<VertexBufferElement>& elements = vertexBufferFormat.getElements();

	Vector<D3D11_INPUT_ELEMENT_DESC> vertexDesc;
	unsigned int offset = 0;
	for (auto& element : elements)
	{
		D3D11_INPUT_ELEMENT_DESC desc({ element.m_Name, 0, (DXGI_FORMAT)element.m_Type, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		offset += VertexBufferElement::GetSize(element.m_Type);

		vertexDesc.push_back(desc);
	}

	RenderingDevice::GetSingleton()->initVertexLayout(vertexDesc);
}

void VertexArray::bind() const
{
}

void VertexArray::unbind() const
{
}
