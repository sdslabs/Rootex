#include "renderer.h"

#include <d3d11.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <array>
#include <iostream>

#include "shader_library.h"

Renderer::Renderer(unsigned int w, unsigned int h)
    : m_Width(w)
    , m_Height(h)
{
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

void Renderer::draw(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer, const Shader* shader) const
{
	VertexBuffer vertexBufferA({ 
		{ -1.0f, -1.0f, -1.0f },
	    { +1.0f, -1.0f, -1.0f },
	    { -1.0f, +1.0f, -1.0f },
	    { +1.0f, +1.0f, -1.0f },
	    { -1.0f, -1.0f, +1.0f },
	    { +1.0f, -1.0f, +1.0f },
	    { -1.0f, +1.0f, +1.0f },
	    { +1.0f, +1.0f, +1.0f } });
	IndexBuffer indexBufferA({ 
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
	    1, 5, 4 });
	
	BufferFormat bufferFormat;
	VSConstantBuffer VSConstantBuffer;
	static float x = 0;
	static float y = 0;
	static float u = 0;
	static float l = 0;
	if (GetAsyncKeyState(VK_UP))
	{
		u += 0.01;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		u += -0.01;
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		l += -0.01;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		l += 0.01;
	}
	x -= l;
	y += u;
	DirectX::XMMATRIX model = DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f) * DirectX::XMMatrixTranslation(x, y, 0.0f);
	DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, 4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveLH(1, 1 * 640 / 480, 0.5f, 10.0f);
	VSConstantBuffer.m_MVP = model * view * projection;
	VSConstantBuffer.m_MVP = DirectX::XMMatrixTranspose(VSConstantBuffer.m_MVP);
		
	PSConstantBuffer PSConstantBuffer;
	PSConstantBuffer.m_Colors[0] = { 1.0f, 0.0f, 0.0f, 1.0f };
	PSConstantBuffer.m_Colors[1] = { 0.0f, 1.0f, 0.0f, 1.0f };
	PSConstantBuffer.m_Colors[2] = { 0.0f, 0.0f, 1.0f, 1.0f };
	PSConstantBuffer.m_Colors[3] = { 0.0f, 0.0f, 1.0f, 1.0f };
	PSConstantBuffer.m_Colors[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	PSConstantBuffer.m_Colors[5] = { 0.0f, 1.0f, 1.0f, 1.0f };
	Shader* shaderA = ShaderLibrary::GetShader("Default");
	shaderA->setConstantBuffer(VSConstantBuffer);
	shaderA->setConstantBuffer(PSConstantBuffer);
	
	vertexBufferA.bind();
	shaderA->bind();
	indexBufferA.bind();

	vertexBuffer.bind();
	indexBuffer.bind();
	shader->bind();

	RenderingDevice::GetSingleton()->drawIndexed(indexBufferA.getCount());
}
