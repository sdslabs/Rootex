#pragma once

#include "common/types.h"

#define MAX_LOD_COUNT 5

class VertexBuffer;
class IndexBuffer;

struct Mesh
{
	Ref<VertexBuffer> m_VertexBuffer;
	BoundingBox m_BoundingBox;

	Vector<Pair<Ref<IndexBuffer>, float>> m_LODs;
	
	Mesh() = default;
	Mesh(const Mesh&) = default;
	~Mesh() = default;

	void addLOD(Ref<IndexBuffer> ib, float lodLevel);

	/// LOD level can range from 0.0f to 1.0f, returns the appropriate LOD
	Ref<IndexBuffer> getLOD(float lodLevel) const;
};
