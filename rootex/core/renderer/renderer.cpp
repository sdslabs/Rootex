#include "renderer.h"

#include <d3d11.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <array>
#include <iostream>

Renderer::Renderer(unsigned int w, unsigned int h)
{
	m_Width = w;
	m_Height = h;
	RenderingDevice::GetSingleton()->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::clear() const
{
	RenderingDevice::GetSingleton()->clearBuffer(0.0f, 0.0f, 0.0f);
}

void Renderer::setViewport(Viewport& viewport)
{
	RenderingDevice::GetSingleton()->setViewport(viewport.getViewport());
}

void Renderer::draw(const IndexBuffer& indexBuffer) const
{
	RenderingDevice::GetSingleton()->drawIndexed(indexBuffer.getCount());
}

void Renderer::drawTest()
{
	static std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();

	// The color change effect ->
	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - startTime;
	float seconds = elapsed_seconds.count();
	float r = (sin(seconds) + 1.0) * 0.5;
	float g = (sin(seconds * 0.3) + 1.0) * 0.5;
	float b = (sin(seconds * 0.7) + 1.0) * 0.5;
	RenderingDevice::GetSingleton()->clearBuffer(r, g, b);

	drawTestCube(seconds);
	drawTestCube(-seconds);
}

void Renderer::drawTestCube(float angle)
{
	struct Vertex
	{
		struct
		{
			float x, y, z;
		} pos;
	};
	const Vertex vertices[] = {
		{ -1.0f, -1.0f, -1.0f },
		{ 1.0f, -1.0f, -1.0f },
		{ -1.0f, 1.0f, -1.0f },
		{ 1.0f, 1.0f, -1.0f },
		{ -1.0f, -1.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f },
	};

	D3D11_BUFFER_DESC vbd = { 0 };
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.CPUAccessFlags = 0u;
	vbd.MiscFlags = 0u;
	vbd.ByteWidth = sizeof(vertices);
	vbd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA vsd = { 0 };
	vsd.pSysMem = vertices;

	//create and bind vertex buffer

	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;

	RenderingDevice::GetSingleton()->initVertexBuffer(&vbd, &vsd, &stride, &offset);

	//create and bind index buffer
	const unsigned short indices[] = {
		0, 2, 1,
		2, 3, 1,
		1, 3, 5,
		3, 7, 5,
		2, 6, 3,
		3, 6, 7,
		4, 5, 7,
		4, 7, 6,
		0, 4, 2,
		2, 4, 6,
		0, 1, 4,
		1, 5, 4
	};
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = { 0 };
	isd.pSysMem = indices;

	RenderingDevice::GetSingleton()->initIndexBuffer(&ibd, &isd, DXGI_FORMAT_R16_UINT);

	struct ConstantBuffer
	{
		DirectX::XMMATRIX transform;
	};

	float maxX = 1.0f;
	float minZ = 0.5f;
	float maxZ = 10.0f;

	//constant buffer(rot matrix) used in vertex shader
	const ConstantBuffer cb = {
		{ DirectX::XMMatrixTranspose(
		    DirectX::XMMatrixRotationZ(angle) * DirectX::XMMatrixRotationY(angle / 2) * DirectX::XMMatrixRotationX(angle / 3) * DirectX::XMMatrixTranslation(0.0f, 0.0f, 4.0f) * DirectX::XMMatrixPerspectiveLH(maxX, maxX * m_Height / m_Width, minZ, maxZ)) }
	};
	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = { 0 };
	csd.pSysMem = &cb;

	RenderingDevice::GetSingleton()->initVSConstantBuffer(&cbd, &csd);

	struct ConstantBuffer2
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} face_colors[6];
	};

	const ConstantBuffer2 cb2 = {
		{ { 1.0f, 0.0f, 0.0f },
		    { 0.0f, 1.0f, 0.0f },
		    { 0.0f, 0.0f, 1.0f },
		    { 1.0f, 1.0f, 0.0f },
		    { 1.0f, 0.0f, 1.0f },
		    { 0.0f, 1.0f, 1.0f } }
	};

	D3D11_BUFFER_DESC cbd2 = { 0 };
	cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd2.Usage = D3D11_USAGE_DYNAMIC;
	cbd2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd2.MiscFlags = 0u;
	cbd2.ByteWidth = sizeof(cb2);
	cbd2.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd2 = { 0 };
	csd2.pSysMem = &cb2;

	RenderingDevice::GetSingleton()->initPSConstantBuffer(&cbd2, &csd2);

	RenderingDevice::GetSingleton()->initPixelShader(L"PixelShader.cso");

	ID3DBlob* vertexShaderBlob = RenderingDevice::GetSingleton()->initVertexShader(L"VertexShader.cso");

	//inout layout 2d
	const D3D11_INPUT_ELEMENT_DESC ied[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	RenderingDevice::GetSingleton()->initVertexLayout(vertexShaderBlob, ied, std::size(ied));

	//set topology
	RenderingDevice::GetSingleton()->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//viewport
	D3D11_VIEWPORT vp;
	vp.Width = m_Width;
	vp.Height = m_Height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	RenderingDevice::GetSingleton()->setViewport(&vp);

	RenderingDevice::GetSingleton()->drawIndexed(std::size(indices));
}
