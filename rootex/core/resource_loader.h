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

static const inline HashMap<ResourceFile::Type, const char*> SupportedFiles = {
	{ ResourceFile::Type::Font, ".spritefont" },
	{ ResourceFile::Type::Model, ".obj,.blend,.3ds,.fbx" },
	{ ResourceFile::Type::AnimatedModel, ".dae,.fbx" },
	{ ResourceFile::Type::CollisionModel, ".obj" },
	{ ResourceFile::Type::Audio, ".wav" },
	{ ResourceFile::Type::Image, ".png,.jpeg,.jpg,.dds" },
	{ ResourceFile::Type::ImageCube, ".dds" },
	{ ResourceFile::Type::Text, ".txt,.json,.rml" },
	{ ResourceFile::Type::Lua, ".lua" },
	{ ResourceFile::Type::None, "" }
};

bool IsFileSupported(const String& extension, ResourceFile::Type supportedFileType);

/// Factory for ResourceFile objects. Implements creating, loading and saving files.                                \n
/// Maintains an internal cache that doesn't let the same file to be loaded twice. Cache misses force file loading. \n
/// This just means you can load the same file multiple times without worrying about unnecessary copies.            \n
/// All path arguments should be relative to Rootex root.
class ResourceLoader
{
	static HashMap<ResourceFile::Type, Vector<Ref<ResourceFile>>> s_ResourcesDataFiles;
	
	template<class T>
	static T* GetCachedResource(ResourceFile::Type type, const FilePath& path);
	static void RegisterResource(Ref<ResourceFile> file);

public:
	static void RegisterAPI(sol::table& rootex);

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
	
	/// Use when you don't know what kind of a resource file will it be
	static ResourceFile* CreateSomeResourceFile(const String& path);
	
	/// Load all the files passed in, in a parellel manner. Return total tasks generated.
	static int Preload(Vector<String> paths, Atomic<int>& progress);
	static void Unload(const Vector<String>& paths);
};

template<class T>
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
