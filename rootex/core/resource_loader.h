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

static const inline HashMap<ResourceFile::Type, const char*> SupportedFiles = {
	{ ResourceFile::Type::Image, "Image(*.png;*.jpeg;*.jpg;*.dds)\0*.png;*.jpeg;*.jpg;*.dds\0" },
	{ ResourceFile::Type::ImageCube, "Image Cube(*.dds)\0*.dds\0" },
	{ ResourceFile::Type::Audio, "Audio(*.wav)\0*.wav\0" },
	{ ResourceFile::Type::Model, "Model(*.3ds;*.fbx;*.dae)\0*.obj;*.blend;*.3ds;*.fbx;*.dae\0" },
	{ ResourceFile::Type::AnimatedModel, "Animated Model(*.dae;*.fbx)\0*.dae;*.fbx\0" },
	{ ResourceFile::Type::CollisionModel, "Collision Model(*.obj)\0*.obj\0" },
	{ ResourceFile::Type::Lua, "Lua(*.lua)\0*.lua\0" },
	{ ResourceFile::Type::Text, "Text(*.txt;*.json;*.rml)\0*.txt;*.json;*.rml\0" },
	{ ResourceFile::Type::Font, "Sprite fonts(*.spritefont)\0*.spritefont\0" },
	{ ResourceFile::Type::ParticleEffect, "Effekseer effects(*.efkefc;*.efk;*.efkproj)\0*.efkefc;*.efk;*.efkproj\0" },
	{ ResourceFile::Type::None, "All(*.*)\0*.*\0" }
};

bool IsFileSupported(const String& extension, ResourceFile::Type supportedFileType);

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
	static const HashMap<ResourceFile::Type, Vector<Weak<ResourceFile>>>& GetResources();
	static void ClearDeadResources();

	static Ref<TextResourceFile> CreateTextResourceFile(const String& path);
	static Ref<TextResourceFile> CreateNewTextResourceFile(const String& path);
	static Ref<LuaTextResourceFile> CreateLuaTextResourceFile(const String& path);
	static Ref<AudioResourceFile> CreateAudioResourceFile(const String& path);
	static Ref<ModelResourceFile> CreateModelResourceFile(const String& path);
	static Ref<CollisionModelResourceFile> CreateCollisionModelResourceFile(const String& path);
	static Ref<AnimatedModelResourceFile> CreateAnimatedModelResourceFile(const String& path);
	static Ref<ImageResourceFile> CreateImageResourceFile(const String& path);
	static Ref<ImageCubeResourceFile> CreateImageCubeResourceFile(const String& path);
	static Ref<FontResourceFile> CreateFontResourceFile(const String& path);
	static Ref<ParticleEffectResourceFile> CreateParticleEffectResourceFile(const String& path);

	static Ref<ResourceFile> CreateResourceFile(const ResourceFile::Type& type, const String& path);

	/// Load all the files passed in parallely. Return total tasks generated.
	static int Preload(ResourceCollection paths, Atomic<int>& progress);

	/// Copy/Generate all export files parallely. Return total tasks generated.
	static int Export(const String& sceneName, const String& sceneFilePath, Atomic<int>& progress);

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
