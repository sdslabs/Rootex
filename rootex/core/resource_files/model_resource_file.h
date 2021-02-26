#pragma once

#include "resource_file.h"
#include "renderer/mesh.h"

class Material;

/// Representation of a 3D model file.
class ModelResourceFile : public ResourceFile
{
	explicit ModelResourceFile(const FilePath& path);

	Vector<Pair<Ref<Material>, Vector<Mesh>>> m_Meshes;

	friend class ResourceLoader;

public:
	explicit ModelResourceFile(const ModelResourceFile&) = delete;
	explicit ModelResourceFile(const ModelResourceFile&&) = delete;
	~ModelResourceFile() = default;

	void reimport() override;

	Vector<Pair<Ref<Material>, Vector<Mesh>>>& getMeshes() { return m_Meshes; }
};
