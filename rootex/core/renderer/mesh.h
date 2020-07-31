#pragma once

#include "index_buffer.h"
#include "vertex_buffer.h"

class BasicMaterial;

struct Mesh
{
	Ref<VertexBuffer> m_VertexBuffer;
	Ref<IndexBuffer> m_IndexBuffer;
	BoundingBox m_BoundingBox;

	Mesh() = default;
	Mesh(const Mesh&) = default;
	~Mesh() = default;
};

struct AnimatedMesh
{
	Ref<VertexBuffer> m_VertexBuffer;
	Ref<IndexBuffer> m_IndexBuffer;
	UINT m_NumBones;

	AnimatedMesh() = default;
	AnimatedMesh(const AnimatedMesh&) = default;
	~AnimatedMesh() = default;
};
