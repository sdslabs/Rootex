#pragma once

#include "index_buffer.h"
#include "vertex_buffer.h"

class BasicMaterial;

struct Mesh
{
	Ref<VertexBuffer> m_VertexBuffer;
	Ref<IndexBuffer> m_IndexBuffer;

	Mesh() = default;
	Mesh(const Mesh&) = default;
	~Mesh() = default;
};
