#pragma once

#include "resource_file.h"
#include "renderer/mesh.h"
#include <Assimp/scene.h>
#include "core/animation/animation.h"

class Material;
class Texture;

/// Representation of an animated 3D model file. Supports .dae files
class AnimatedModelResourceFile : public ResourceFile
{
	explicit AnimatedModelResourceFile(const FilePath& path);

	HashMap<Ref<Material>, Vector<AnimatedMesh>> m_Meshes;
    Vector<Ref<Texture>> m_Textures;
    HashMap<String, UINT> m_BoneMapping;
	Vector<Matrix> m_BoneOffsets;
	Vector<Matrix> m_BoneTransforms;
    HashMap<String, SkeletalAnimation> m_Animations;

	friend class ResourceLoader;

public:
	static void RegisterAPI(sol::table& rootex);

	explicit AnimatedModelResourceFile(const AnimatedModelResourceFile&) = delete;
	explicit AnimatedModelResourceFile(const AnimatedModelResourceFile&&) = delete;
    ~AnimatedModelResourceFile() = default;

    void reimport() override;
    
    HashMap<String, SkeletalAnimation>& getAnimations() { return m_Animations; }
	UINT getBoneCount() const { return m_BoneOffsets.size(); }

	Vector<String> getAnimationNames();
	float getAnimationEndTime(const String& animationName);
	void setBoneTransforms(aiNode* currentNode, Matrix rootTransform);
	void getFinalTransforms(const String& animationName, float currentTime, Vector<Matrix>& transforms);
	HashMap<Ref<Material>, Vector<AnimatedMesh>>& getMeshes() { return m_Meshes; }
};
