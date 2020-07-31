#pragma once

#include "resource_file.h"
#include "renderer/mesh.h"

class Material;
class Texture;

/// Representation of an animated 3D model file. Supports .dae files
class AnimatedModelResourceFile : public ResourceFile
{
	explicit AnimatedModelResourceFile(const FilePath& path);

	HashMap<Ref<Material>, Vector<AnimatedMesh>> m_Meshes;
    Vector<Ref<Texture>> m_Textures;

	friend class ResourceLoader;

public:
	static void RegisterAPI(sol::table& rootex);

	explicit AnimatedModelResourceFile(const AnimatedModelResourceFile&) = delete;
	explicit AnimatedModelResourceFile(const AnimatedModelResourceFile&&) = delete;
    ~AnimatedModelResourceFile() = default;

    void reimport() override;

	HashMap<Ref<Material>, Vector<AnimatedMesh>>& getMeshes() { return m_Meshes; }
};
