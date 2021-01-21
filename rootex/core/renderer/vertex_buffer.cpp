#include "vertex_buffer.h"

#include "rendering_device.h"

#include "Tracy/Tracy.hpp"

VertexBuffer::VertexBuffer(const Vector<VertexData>& buffer)
    : m_Stride(sizeof(VertexData))
    , m_Count(buffer.size())
{
	D3D11_BUFFER_DESC vbd = { 0 };
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0u;
	vbd.ByteWidth = sizeof(VertexData) * buffer.size();
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vsd = { 0 };
	vsd.pSysMem = buffer.data();
	
	const UINT offset = 0u;
	m_VertexBuffer = RenderingDevice::GetSingleton()->createBuffer(&vbd, &vsd);
}

VertexBuffer::VertexBuffer(const Vector<InstanceData>& buffer)
    : m_Stride(sizeof(InstanceData))
    , m_Count(buffer.size())
{
	D3D11_BUFFER_DESC vbd = { 0 };
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0u;
	vbd.ByteWidth = sizeof(InstanceData) * buffer.size();
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vsd = { 0 };
	vsd.pSysMem = buffer.data();

	const UINT offset = 0u;
	m_VertexBuffer = RenderingDevice::GetSingleton()->createBuffer(&vbd, &vsd);
}

VertexBuffer::VertexBuffer(const Vector<UIVertexData>& buffer)
    : m_Stride(sizeof(UIVertexData))
    , m_Count(buffer.size())
{
	D3D11_BUFFER_DESC vbd = { 0 };
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0u;
	vbd.ByteWidth = sizeof(UIVertexData) * buffer.size();
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vsd = { 0 };
	vsd.pSysMem = buffer.data();

	const UINT offset = 0u;
	m_VertexBuffer = RenderingDevice::GetSingleton()->createBuffer(&vbd, &vsd);
}

VertexBuffer::VertexBuffer(const Vector<AnimatedVertexData>& buffer)
    : m_Stride(sizeof(AnimatedVertexData))
    , m_Count(buffer.size())
{
	D3D11_BUFFER_DESC vbd = { 0 };
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0u;
	vbd.ByteWidth = sizeof(AnimatedVertexData) * buffer.size();
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vsd = { 0 };
	vsd.pSysMem = buffer.data();

	const UINT offset = 0u;
	m_VertexBuffer = RenderingDevice::GetSingleton()->createBuffer(&vbd, &vsd);
}

VertexBuffer::VertexBuffer(const Vector<FXAAData>& buffer)
    : m_Stride(sizeof(FXAAData))
    , m_Count(buffer.size())
{
	D3D11_BUFFER_DESC vbd = { 0 };
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0u;
	vbd.ByteWidth = sizeof(FXAAData) * buffer.size();
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vsd = { 0 };
	vsd.pSysMem = buffer.data();

	const UINT offset = 0u;
	m_VertexBuffer = RenderingDevice::GetSingleton()->createBuffer(&vbd, &vsd);
}

VertexBuffer::VertexBuffer(const Vector<float>& buffer)
    : m_Stride(3 * sizeof(float))
    , m_Count(buffer.size() / 3)
{
	D3D11_BUFFER_DESC vbd = { 0 };
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0u;
	vbd.ByteWidth = sizeof(float) * buffer.size();
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vsd = { 0 };
	vsd.pSysMem = buffer.data();

	const UINT offset = 0u;
	m_VertexBuffer = RenderingDevice::GetSingleton()->createBuffer(&vbd, &vsd);
}

void VertexBuffer::bind() const
{
	ZoneNamedN(bufferBind, "VB Bind", true);
	const UINT offset = 0u;
	RenderingDevice::GetSingleton()->bind(m_VertexBuffer.GetAddressOf(), 1, &m_Stride, &offset);
}

void VertexBuffer::setData(const Vector<InstanceData>& buffer)
{
	D3D11_MAPPED_SUBRESOURCE subresource = { 0 };
	RenderingDevice::GetSingleton()->mapBuffer(m_VertexBuffer.Get(), subresource);
	memcpy(subresource.pData, buffer.data(), buffer.size() * sizeof(InstanceData));
	RenderingDevice::GetSingleton()->unmapBuffer(m_VertexBuffer.Get());
	m_Count = buffer.size();
}
