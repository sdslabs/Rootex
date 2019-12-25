#include "index_buffer.h"

#include "rendering_device.h"
#include "utils.h"

IndexBuffer::IndexBuffer()
    : m_Buffer({ 0, 2, 1 })
{
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = m_Buffer.size() * sizeof(unsigned short);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = { 0 };
	isd.pSysMem = m_Buffer.data();

	m_IndexBuffer = RenderingDevice::GetSingleton()->initIndexBuffer(&ibd, &isd, DXGI_FORMAT_R16_UINT);
}

IndexBuffer::IndexBuffer(const Vector<unsigned short>& indices)
    : m_Buffer(indices)
{
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = indices.size() * sizeof(unsigned short);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = { 0 };
	isd.pSysMem = indices.data();

	m_IndexBuffer = RenderingDevice::GetSingleton()->initIndexBuffer(&ibd, &isd, DXGI_FORMAT_R16_UINT);
}

IndexBuffer::~IndexBuffer()
{
	SafeRelease(&m_IndexBuffer);
}

void IndexBuffer::bind() const
{
	RenderingDevice::GetSingleton()->bind(m_IndexBuffer, DXGI_FORMAT_R16_UINT);
}

unsigned int IndexBuffer::getCount() const
{
	return m_Buffer.size();
}
