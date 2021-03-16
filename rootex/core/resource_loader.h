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
/// All path arguments should be relative to Rootex root.
class ResourceLoader
{
	static HashMap<ResourceFile::Type, Vector<Ref<ResourceFile>>> s_ResourcesDataFiles;
	static Mutex s_ResourceDataMutex;

	template <class T>
	static T* GetCachedResource(ResourceFile::Type type, const FilePath& path);

	/// Threadsafe method to add a file to the resource cache
	static void RegisterResource(Ref<ResourceFile> file);

public:
	static const HashMap<ResourceFile::Type, Vector<Ref<ResourceFile>>>& GetResources() { return s_ResourcesDataFiles; };

	static TextResourceFile* CreateTextResourceFile(const String& path);
	static TextResourceFile* CreateNewTextResourceFile(const String& path);
	static LuaTextResourceFile* CreateLuaTextResourceFile(const String& path);
	static AudioResourceFile* CreateAudioResourceFile(const String& path);
	static ModelResourceFile* CreateModelResourceFile(const String& path);
	static CollisionModelResourceFile* CreateCollisionModelResourceFile(const String& path);
	static AnimatedModelResourceFile* CreateAnimatedModelResourceFile(const String& path);
	static ImageResourceFile* CreateImageResourceFile(const String& path);
	static ImageCubeResourceFile* CreateImageCubeResourceFile(const String& path);
	static FontResourceFile* CreateFontResourceFile(const String& path);
	static ParticleEffectResourceFile* CreateParticleEffectResourceFile(const String& path);

	static ResourceFile* CreateResourceFile(const ResourceFile::Type& type, const String& path);

	/// Load all the files passed in, in a parellel manner. Return total tasks generated.
	static int Preload(ResourceCollection paths, Atomic<int>& progress);
	static void Unload(const ResourceCollection& paths);
};

template <class T>
inline T* ResourceLoader::GetCachedResource(ResourceFile::Type type, const FilePath& path)
{
	String searchPath = path.generic_string();
	for (auto& item : s_ResourcesDataFiles[type])
	{
		if (item->getPath() == searchPath)
		{
			return (T*)(item.get());
		}
	}

	if (!OS::IsExists(searchPath))
	{
		ERR("File not found: " + searchPath);
		return nullptr;
	}

	// File not found in cache, load it only once
	Ref<T> file(new T(searchPath));
	RegisterResource(file);
	return file.get();
}
