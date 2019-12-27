#pragma once
#include "common/common.h"
#include "core/renderer/buffer_format.h"
#include "core/renderer/index_buffer.h"
#include "core/renderer/renderer.h"
#include "core/renderer/shader.h"
#include "core/renderer/vertex_buffer.h"
#include "core/resource_loader.h"
#include "renderer/material.h"

//#include "scene/scene.h"

class RenderableObject
{
protected:
	std::shared_ptr<Material> material;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	//VertexBuffer* vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;
	//IndexBuffer* indexBuffer;
	BufferFormat bufferFormat;
	std::unique_ptr<PSConstantBuffer> psConstantBuffer;
	std::unique_ptr<Shader> shader;

public:

	virtual void Update(const AlignedMatrix& transform) {};
	virtual void Draw() {
		 vertexBuffer->bind();
		 indexBuffer->bind();
		 shader->bind();
		 RenderingDevice::GetSingleton()->drawIndexed(indexBuffer->getCount());
	}
};
