#include "index_buffer.h"

#include "rendering_device.h"

#include "Tracy/Tracy.hpp"

IndexBuffer::IndexBuffer(const Vector<unsigned short>& indices)
    : m_Count(indices.size())
{
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DYNAMIC;
	ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = indices.size() * sizeof(unsigned short);
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA isd = { 0 };
	isd.pSysMem = indices.data();

	m_Format = DXGI_FORMAT_R16_UINT;
	m_IndexBuffer = RenderingDevice::GetSingleton()->createBuffer(&ibd, &isd);
}

IndexBuffer::IndexBuffer(const Vector<unsigned int>& indices)
    : m_Count(indices.size())
{
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DYNAMIC;
	ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = indices.size() * sizeof(int);
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA isd = { 0 };
	isd.pSysMem = indices.data();

	m_Format = DXGI_FORMAT_R32_UINT;
	m_IndexBuffer = RenderingDevice::GetSingleton()->createBuffer(&ibd, &isd);
}

void IndexBuffer::setData(const Vector<unsigned short>& indices)
{
	D3D11_MAPPED_SUBRESOURCE subresource = { 0 };
	RenderingDevice::GetSingleton()->mapBuffer(m_IndexBuffer.Get(), subresource);
	memcpy(subresource.pData, indices.data(), indices.size() * sizeof(unsigned short));
	RenderingDevice::GetSingleton()->unmapBuffer(m_IndexBuffer.Get());
	m_Count = indices.size();
}

void IndexBuffer::setData(const Vector<unsigned int>& indices)
{
	D3D11_MAPPED_SUBRESOURCE subresource = { 0 };
	RenderingDevice::GetSingleton()->mapBuffer(m_IndexBuffer.Get(), subresource);
	memcpy(subresource.pData, indices.data(), indices.size() * sizeof(unsigned int));
	RenderingDevice::GetSingleton()->unmapBuffer(m_IndexBuffer.Get());
	m_Count = indices.size();
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
