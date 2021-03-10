#include "mesh.h"

#include "index_buffer.h"
#include "vertex_buffer.h"

void Mesh::addLOD(Ref<IndexBuffer> ib, float lodLevel)
{
	m_LODs.push_back({ ib, lodLevel });

	// LODs are to be kept in descending order
	std::sort(m_LODs.begin(), m_LODs.end(), [](const Pair<Ref<IndexBuffer>, float>& a, const Pair<Ref<IndexBuffer>, float>& b) -> bool {
		return a.second > b.second;
	});
}

Ref<IndexBuffer> Mesh::getLOD(float lodLevel) const
{
	for (auto& [meshLOD, meshLODLevel] : m_LODs)
	{
		if (abs((lodLevel - meshLODLevel)) < DirectX::g_XMEpsilon[0])
		{
			return meshLOD;
		}
	}
	return m_LODs.back().first;
}
