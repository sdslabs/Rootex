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

void Renderer::draw(const IndexBuffer& indexBuffer, const Shader& shader) const
{
	shader.bind();
	RenderingDevice::GetSingleton()->drawIndexed(indexBuffer.getCount());
}
