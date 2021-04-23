#include "index_buffer.h"

#include "rendering_device.h"

#include "Tracy/Tracy.hpp"

IndexBuffer::IndexBuffer(const Vector<unsigned short>& indices, bool dynamicWrite)
    : m_Count(indices.size())
    , m_Format(DXGI_FORMAT_R16_UINT)
{
	D3D11_USAGE usageFlag = D3D11_USAGE_IMMUTABLE;
	int cpuAccess = 0;
	if (dynamicWrite)
	{
		usageFlag = D3D11_USAGE_DYNAMIC;
		cpuAccess = D3D11_CPU_ACCESS_WRITE;
	}

	m_IndexBuffer = RenderingDevice::GetSingleton()->createBuffer((const char*)indices.data(), sizeof(unsigned short) * indices.size(), D3D11_BIND_INDEX_BUFFER, usageFlag, cpuAccess);
}

IndexBuffer::IndexBuffer(const Vector<unsigned int>& indices)
    : m_Count(indices.size())
    , m_Format(DXGI_FORMAT_R32_UINT)
{
	m_IndexBuffer = RenderingDevice::GetSingleton()->createBuffer((const char*)indices.data(), sizeof(unsigned int) * indices.size(), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE, 0);
}

IndexBuffer::IndexBuffer(const int* indices, size_t size)
    : m_Count(size)
    , m_Format(DXGI_FORMAT_R32_UINT)
{
	m_IndexBuffer = RenderingDevice::GetSingleton()->createBuffer((const char*)indices, sizeof(int) * size, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE, 0);
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
