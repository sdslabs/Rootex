#include "vertex_buffer.h"

#include "rendering_device.h"
#include "utils.h"
/*
VertexBuffer::VertexBuffer()
    : m_Buffer({
	    // Position              // TexCoords	
		{ { -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f } },
        { { +1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f } },
        { { -1.0f, +1.0f, -1.0f }, { 1.0f, 0.0f } },
        { { +1.0f, +1.0f, -1.0f }, { 0.0f, 0.0f } },
        { { -1.0f, -1.0f, +1.0f }, { 0.0f, 1.0f } },
        { { +1.0f, -1.0f, +1.0f }, { 1.0f, 1.0f } },
        { { -1.0f, +1.0f, +1.0f }, { 0.0f, 0.0f } },
        { { +1.0f, +1.0f, +1.0f }, { 1.0f, 0.0f } } })
{
	D3D11_BUFFER_DESC vbd = { 0 };
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.CPUAccessFlags = 0u;
	vbd.MiscFlags = 0u;
	vbd.ByteWidth = sizeof(VertexData) * m_Buffer.size();
	vbd.StructureByteStride = sizeof(VertexData);
	D3D11_SUBRESOURCE_DATA vsd = { 0 };
	vsd.pSysMem = m_Buffer.data();

	const UINT stride = sizeof(VertexData);
	const UINT offset = 0u;

	m_VertexBuffer = RenderingDevice::GetSingleton()->initVertexBuffer(&vbd, &vsd, &stride, &offset);
}*/

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

	m_VertexBuffer = RenderingDevice::GetSingleton()->initVertexBuffer(&vbd, &vsd, &stride, &offset);
}

void VertexBuffer::bind() const
{
	const UINT stride = sizeof(VertexData);
	const UINT offset = 0u;

	RenderingDevice::GetSingleton()->bind(m_VertexBuffer, &stride, &offset);
}

VertexBuffer::~VertexBuffer()
{
	SafeRelease(&m_VertexBuffer);
}
