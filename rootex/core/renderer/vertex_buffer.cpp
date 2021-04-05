#include "vertex_buffer.h"

#include "rendering_device.h"

#include "Tracy/Tracy.hpp"

VertexBuffer::VertexBuffer(const char* buffer, unsigned int elementCount, unsigned int stride, D3D11_USAGE usage, int cpuAccess)
    : m_Stride(stride)
    , m_Count(elementCount)
{
	m_VertexBuffer = RenderingDevice::GetSingleton()->createBuffer(
	    buffer,
	    stride * elementCount,
	    D3D11_BIND_VERTEX_BUFFER,
	    usage,
	    cpuAccess);
}

void VertexBuffer::bind() const
{
	ZoneNamedN(bufferBind, "VB Bind", true);
	const UINT offset = 0u;
	RenderingDevice::GetSingleton()->bind(m_VertexBuffer.GetAddressOf(), 1, &m_Stride, &offset);
}
