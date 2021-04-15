#pragma once

#include "renderer/mesh.h"
#include "resource_file.h"

#include "btBulletCollisionCommon.h"

/// Representation of a 3D model file used to initialise a mesh collider.
class CollisionModelResourceFile : public ResourceFile
{
	explicit CollisionModelResourceFile(const FilePath& path);

	btTriangleIndexVertexArray m_TriangleMesh;
	Vector<btIndexedMesh> m_Meshes;
	Vector<Vector3> m_Vertices;
	Vector<short> m_Indices;

	friend class ResourceLoader;

public:
	explicit CollisionModelResourceFile(const CollisionModelResourceFile&) = delete;
	explicit CollisionModelResourceFile(const CollisionModelResourceFile&&) = delete;
	~CollisionModelResourceFile() = default;

	void reimport() override;

	btTriangleIndexVertexArray* getCollisionMesh() { return &m_TriangleMesh; }
};
