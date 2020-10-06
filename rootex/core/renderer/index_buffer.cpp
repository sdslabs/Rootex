#include "index_buffer.h"

#include "rendering_device.h"

#include "Tracy/Tracy.hpp"

IndexBuffer::IndexBuffer(const Vector<unsigned short>& indices)
    : m_Count(indices.size())
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

	m_Format = DXGI_FORMAT_R16_UINT;
	m_IndexBuffer = RenderingDevice::GetSingleton()->createIB(&ibd, &isd, m_Format);
}

IndexBuffer::IndexBuffer(const Vector<int>& indices)
    : m_Count(indices.size())
{
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = indices.size() * sizeof(int);
	ibd.StructureByteStride = sizeof(int);
	D3D11_SUBRESOURCE_DATA isd = { 0 };
	isd.pSysMem = indices.data();

	m_Format = DXGI_FORMAT_R32_UINT;
	m_IndexBuffer = RenderingDevice::GetSingleton()->createIB(&ibd, &isd, m_Format);
}

void IndexBuffer::bind() const
{
	ZoneNamedN(bufferBind, "IB Bind", true);
	RenderingDevice::GetSingleton()->bind(m_IndexBuffer.Get(), m_Format);
}

unsigned int IndexBuffer::getCount() const
{
	return m_Count;
}
