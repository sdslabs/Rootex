#pragma once

#include "common/common.h"

#include "resource_file.h"

#include "resource_files/audio_resource_file.h"
#include "resource_files/font_resource_file.h"
#include "resource_files/image_resource_file.h"
#include "resource_files/image_cube_resource_file.h"
#include "resource_files/lua_text_resource_file.h"
#include "resource_files/model_resource_file.h"
#include "resource_files/collision_model_resource_file.h"
#include "resource_files/text_resource_file.h"
#include "resource_files/animated_model_resource_file.h"
#include "resource_files/particle_effect_resource_file.h"
#include "resource_files/basic_material_resource_file.h"
#include "resource_files/instancing_basic_material_resource_file.h"
#include "resource_files/animated_basic_material_resource_file.h"
#include "resource_files/sky_material_resource_file.h"
#include "resource_files/custom_material_resource_file.h"
#include "resource_files/decal_material_resource_file.h"

static const inline HashMap<ResourceFile::Type, const char*> SupportedFiles = {
	{ ResourceFile::Type::Image, "Image(*.png;*.jpeg;*.jpg;*.dds)\0*.png;*.jpeg;*.jpg;*.dds\0" },
	{ ResourceFile::Type::ImageCube, "Image Cube(*.dds)\0*.dds\0" },
	{ ResourceFile::Type::Audio, "Audio(*.wav)\0*.wav\0" },
	{ ResourceFile::Type::Model, "Model(*.3ds;*.fbx;*.dae)\0*.obj;*.blend;*.3ds;*.fbx;*.dae\0" },
	{ ResourceFile::Type::AnimatedModel, "Animated Model(*.dae;*.fbx;*.gltf)\0*.dae;*.fbx;*.gltf\0" },
	{ ResourceFile::Type::CollisionModel, "Collision Model(*.obj)\0*.obj\0" },
	{ ResourceFile::Type::Lua, "Lua(*.lua)\0*.lua\0" },
	{ ResourceFile::Type::Text, "Text(*.txt;*.json;*.rml)\0*.txt;*.json;*.rml\0" },
	{ ResourceFile::Type::Font, "Sprite fonts(*.spritefont)\0*.spritefont\0" },
	{ ResourceFile::Type::ParticleEffect, "Effekseer effects(*.efkefc;*.efk;*.efkproj)\0*.efkefc;*.efk;*.efkproj\0" },
	{ ResourceFile::Type::BasicMaterial, "Basic material(*.basic.rmat)\0*.basic.rmat\0" },
	{ ResourceFile::Type::InstancingBasicMaterial, "Instancing basic material(*.instance.rmat)\0*.instance.rmat\0" },
	{ ResourceFile::Type::AnimatedBasicMaterial, "Animated material(*.anim.rmat)\0*.anim.rmat\0" },
	{ ResourceFile::Type::SkyMaterial, "Sky material(*.sky.rmat)\0*.sky.rmat\0" },
	{ ResourceFile::Type::CustomMaterial, "Custom material(*.custom.rmat)\0*.custom.rmat\0" },
	{ ResourceFile::Type::DecalMaterial, "Decal material(*.decal.rmat)\0*.decal.rmat\0" },
	{ ResourceFile::Type::None, "All(*.*)\0*.*\0" }
};

static const inline HashMap<ResourceFile::Type, const char*> CreatableFiles = {
	{ ResourceFile::Type::Lua, ".lua" },
	{ ResourceFile::Type::Text, ".txt" },
	{ ResourceFile::Type::BasicMaterial, ".basic.rmat" },
	{ ResourceFile::Type::InstancingBasicMaterial, ".instance.rmat" },
	{ ResourceFile::Type::AnimatedBasicMaterial, ".anim.rmat" },
	{ ResourceFile::Type::SkyMaterial, ".sky.rmat" },
	{ ResourceFile::Type::CustomMaterial, ".custom.rmat" },
	{ ResourceFile::Type::DecalMaterial, ".decal.rmat" },
};

static const inline HashMap<String, Vector<const char*>> m_PayloadTypes = {
	{ ".png", { "IMAGE_PAYLOAD" } },
	{ ".jpg", { "IMAGE_PAYLOAD" } },
	{ ".jpeg", { "IMAGE_PAYLOAD" } },
	{ ".dds", { "IMAGE_PAYLOAD", "IMAGE_CUBE_PAYLOAD" } },
	{ ".wav", { "AUDIO_PAYLOAD" } },
	{ ".3ds", { "MODEL_PAYLOAD" } },
	{ ".fbx", { "MODEL_PAYLOAD", "ANIMATED_MODEL_PAYLOAD" } },
	{ ".obj", { "MODEL_PAYLOAD", "COLLISION_MODEL_PAYLOAD" } },
	{ ".blend", { "MODEL_PAYLOAD" } },
	{ ".dae", { "MODEL_PAYLOAD", "ANIMATED_MODEL_PAYLOAD" } },
	{ ".gltf", { "ANIMATED_MODEL_PAYLOAD" } },
	{ ".lua", { "SCRIPT_PAYLOAD" } },
	{ ".rml", { "RML_PAYLOAD" } },
	{ ".rmat", { "MATERIAL_PAYLOAD" } },
	{ ".json", { "SCENE_PAYLOAD" } },
};

bool IsFileSupported(const String& extension, ResourceFile::Type supportedFileType);
Vector<const char*> GetPayloadTypes(const String& extension);

/// Factory for ResourceFile objects. Implements creating, loading and saving files.                                \n
/// Maintains an internal cache that doesn't let the same file to be loaded twice. Cache misses force file loading. \n
/// This just means you can load the same file multiple times without worrying about unnecessary copies.            \n
/// All path arguments should be relative to Rootex root.                                                           \n
/// The resource creation API is internally synchronised (threadsafe).
class ResourceLoader
{
	static inline HashMap<ResourceFile::Type, Vector<Weak<ResourceFile>>> s_ResourcesDataFiles;
	static inline Vector<Ref<ResourceFile>> s_PersistentResources;

	static inline RecursiveMutex s_PersistMutex;
	static inline RecursiveMutex s_ResourceDataMutex;

	template <class T>
	static Ref<T> GetCachedResource(ResourceFile::Type type, const FilePath& path);

public:
	static void Initialize();
	static void Destroy();
	static const HashMap<ResourceFile::Type, Vector<Weak<ResourceFile>>>& GetResources();
	static const char* GetCreatableExtension(ResourceFile::Type type);
	static void SaveResources(ResourceFile::Type type);
	static void ClearDeadResources();

	static Ref<TextResourceFile> CreateTextResourceFile(const String& path);
	static Ref<LuaTextResourceFile> CreateLuaTextResourceFile(const String& path);
	static Ref<AudioResourceFile> CreateAudioResourceFile(const String& path);
	static Ref<ModelResourceFile> CreateModelResourceFile(const String& path);
	static Ref<CollisionModelResourceFile> CreateCollisionModelResourceFile(const String& path);
	static Ref<AnimatedModelResourceFile> CreateAnimatedModelResourceFile(const String& path);
	static Ref<ImageResourceFile> CreateImageResourceFile(const String& path);
	static Ref<ImageCubeResourceFile> CreateImageCubeResourceFile(const String& path);
	static Ref<FontResourceFile> CreateFontResourceFile(const String& path);
	static Ref<ParticleEffectResourceFile> CreateParticleEffectResourceFile(const String& path);
	static Ref<MaterialResourceFile> CreateMaterialResourceFile(const String& path);
	static Ref<BasicMaterialResourceFile> CreateBasicMaterialResourceFile(const String& path);
	static Ref<InstancingBasicMaterialResourceFile> CreateInstancingBasicMaterialResourceFile(const String& path);
	static Ref<AnimatedBasicMaterialResourceFile> CreateAnimatedBasicMaterialResourceFile(const String& path);
	static Ref<SkyMaterialResourceFile> CreateSkyMaterialResourceFile(const String& path);
	static Ref<CustomMaterialResourceFile> CreateCustomMaterialResourceFile(const String& path);
	static Ref<DecalMaterialResourceFile> CreateDecalMaterialResourceFile(const String& path);

	static Ref<ResourceFile> CreateResourceFile(const ResourceFile::Type& type, const String& path);

	static Ref<TextResourceFile> CreateNewTextResourceFile(const String& path);
	static Ref<BasicMaterialResourceFile> CreateNewBasicMaterialResourceFile(const String& path);
	static Ref<AnimatedBasicMaterialResourceFile> CreateNewAnimatedBasicMaterialResourceFile(const String& path);

	/// Load all the files passed in parallely. Return total tasks generated.
	static int Preload(ResourceCollection paths, Atomic<int>& progress);

	/// Add a resource to be kept alive till explicitly ordered to clear them. Internally synchronised.
	static void Persist(Ref<ResourceFile> res);
	static void ClearPersistentResources();
};

template <class T>
inline Ref<T> ResourceLoader::GetCachedResource(ResourceFile::Type type, const FilePath& path)
{
	s_ResourceDataMutex.lock();
	String searchPath = path.generic_string();
	Ref<T> ret;
	for (auto& item : s_ResourcesDataFiles[type])
	{
		if (item.lock())
		{
			Ref<ResourceFile> itemRes = item.lock();
			if (itemRes && itemRes->getPath() == searchPath)
			{
				ret = std::dynamic_pointer_cast<T>(itemRes);
				break;
			}
		}
	}
	s_ResourceDataMutex.unlock();

	if (ret)
	{
		return ret;
	}

	// File not found in cache, load it
	if (!OS::IsExists(searchPath))
	{
		ERR("File not found: " + searchPath);
		return nullptr;
	}
	Ref<T> file(new T(searchPath));

	s_ResourceDataMutex.lock();
	s_ResourcesDataFiles[file->getType()].push_back(file);
	s_ResourceDataMutex.unlock();

	return file;
}
