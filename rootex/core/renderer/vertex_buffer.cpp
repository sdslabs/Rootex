#include "vertex_buffer.h"

#include "rendering_device.h"

VertexBuffer::VertexBuffer(const Vector<VertexData>& buffer)
    : m_Buffer(buffer)
{
	D3D11_BUFFER_DESC vbd = { 0 };
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.CPUAccessFlags = 0u;
	vbd.MiscFlags = 0u;
	vbd.ByteWidth = sizeof(VertexData) * buffer.size();
	vbd.StructureByteStride = sizeof(VertexData);
	D3D11_SUBRESOURCE_DATA vsd = { 0 };
	vsd.pSysMem = buffer.data();
	
	const UINT stride = sizeof(VertexData);
	const UINT offset = 0u;

	m_VertexBuffer = RenderingDevice::GetSingleton()->createVertexBuffer(&vbd, &vsd, &stride, &offset);
}

void VertexBuffer::bind() const
{
	const UINT stride = sizeof(VertexData);
	const UINT offset = 0u;

	RenderingDevice::GetSingleton()->bind(m_VertexBuffer.Get(), &stride, &offset);
}

VertexBuffer::~VertexBuffer()
{
}
