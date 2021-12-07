#pragma once

#include "resource_file.h"
#include "basic_material_resource_file.h"

class ResourceFile;
class Mesh;

/// Representation of a 3D model file.
class ModelResourceFile : public ResourceFile
{
	explicit ModelResourceFile(const FilePath& path);

	Vector<Pair<Ref<BasicMaterialResourceFile>, Vector<Mesh>>> m_Meshes;

	friend class ResourceLoader;

public:
	explicit ModelResourceFile(const ModelResourceFile&) = delete;
	explicit ModelResourceFile(const ModelResourceFile&&) = delete;
	~ModelResourceFile() = default;

	void reimport() override;

	Vector<Pair<Ref<BasicMaterialResourceFile>, Vector<Mesh>>>& getMeshes() { return m_Meshes; }
	int getMaterialCount() { return m_Meshes.size(); }
	Ref<BasicMaterialResourceFile> getMaterialAt(int i) { return m_Meshes[i].first; }
	Vector<Mesh>& getMeshesOfMaterialAt(int i) { return m_Meshes[i].second; }
};
