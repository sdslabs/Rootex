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

void Renderer::draw(const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer, const Material* material, const Texture* texture) const
{
	vertexBuffer->bind();
	indexBuffer->bind();
	material->bindShaderTexture(texture);
	material->bind();

	RenderingDevice::GetSingleton()->drawIndexed(indexBuffer->getCount());
}
