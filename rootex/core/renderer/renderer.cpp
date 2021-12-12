#include "renderer.h"

#include <d3d11.h>
#include "core/resource_files/basic_material_resource_file.h"
#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <array>
#include <iostream>

#include "Tracy/Tracy.hpp"

Renderer::Renderer()
    : m_CurrentShader(nullptr)
{
	RenderingDevice::GetSingleton()->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::setViewport(Viewport& viewport)
{
	RenderingDevice::GetSingleton()->setViewport(viewport.getViewport());
}

void Renderer::resetCurrentShader()
{
	m_CurrentShader = nullptr;
}

void Renderer::bind(MaterialResourceFile* newMaterial, MaterialResourceFile* oldMaterial)
{
	BasicMaterialResourceFile* BasicMaterialResourceFilePointer = dynamic_cast<BasicMaterialResourceFile*>(newMaterial);
	if (BasicMaterialResourceFilePointer != nullptr)
	{
		ZoneNamedN(materialBind, "Render Material Bind", true);
		if (newMaterial->getShader() != m_CurrentShader)
		{
			ZoneNamedN(materialBind, "Shader Bind", true);
			m_CurrentShader = newMaterial->getShader();
			newMaterial->bindShader();
		}
		newMaterial->bindSamplers();
		newMaterial->bindTextures();
		newMaterial->bindVSCB();
		newMaterial->bindPSCB();
	}
	else
	{
		ZoneNamedN(materialBind, "Render Material Bind", true);
		if (newMaterial->getShader() != m_CurrentShader)
		{
			ZoneNamedN(materialBind, "Shader Bind", true);
			m_CurrentShader = newMaterial->getShader();
			newMaterial->bindShader();
		}
		oldMaterial->bindSamplers();
		oldMaterial->bindTextures();
		oldMaterial->bindVSCB();
		oldMaterial->bindPSCB();
	}
}
void Renderer::bind(MaterialResourceFile* material)
{
	ZoneNamedN(materialBind, "Render Material Bind", true);
	if (material->getShader() != m_CurrentShader)
	{
		ZoneNamedN(materialBind, "Shader Bind", true);
		m_CurrentShader = material->getShader();
		material->bindShader();
	}
	material->bindSamplers();
	material->bindTextures();
	material->bindVSCB();
	material->bindPSCB();
}

void Renderer::draw(const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer) const
{
	vertexBuffer->bind();
	indexBuffer->bind();
	RenderingDevice::GetSingleton()->drawIndexed(indexBuffer->getCount());
}

void Renderer::drawInstanced(const VertexBuffer* vertexBuffer, const IndexBuffer* indexBuffer, const VertexBuffer* instanceBuffer, unsigned int instances) const
{
	ID3D11Buffer* buffers[2] = { vertexBuffer->getBuffer(), instanceBuffer->getBuffer() };
	unsigned int strides[2] = { vertexBuffer->getStride(), instanceBuffer->getStride() };
	unsigned int offsets[2] = { 0, 0 };
	RenderingDevice::GetSingleton()->bind(buffers, 2, strides, offsets);
	indexBuffer->bind();
	RenderingDevice::GetSingleton()->drawIndexedInstanced(indexBuffer->getCount(), instances, 0);
}
