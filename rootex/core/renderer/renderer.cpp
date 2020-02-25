#include "renderer.h"

#include <d3d11.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <array>
#include <iostream>

#include "shader_library.h"

Renderer::Renderer()
{
	RenderingDevice::GetSingleton()->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::setViewport(Viewport& viewport)
{
	RenderingDevice::GetSingleton()->setViewport(viewport.getViewport());
}

void Renderer::draw(const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer, const Material* material) const
{
	vertexBuffer->bind();
	indexBuffer->bind();
	material->bind();

	RenderingDevice::GetSingleton()->drawIndexed(indexBuffer->getCount());
}
