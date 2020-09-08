#pragma once

#include "common/common.h"
#include "core/resource_data.h"
#include "core/resource_file.h"
#include "os/os.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

static const inline HashMap<ResourceFile::Type, Vector<String>> SupportedFiles = {
	{
	    ResourceFile::Type::Font,
	    { ".spritefont" },
	},
	{
	    ResourceFile::Type::Model,
	    { ".obj", ".blend", ".3ds", ".fbx" },
	},
	{
	    ResourceFile::Type::Audio,
	    { ".wav" },
	},
	{
	    ResourceFile::Type::Image,
	    { ".png", ".jpeg", "jpg" },
	},
	{
	    ResourceFile::Type::Text,
	    { ".txt", ".json" },
	},
	{
	    ResourceFile::Type::Lua,
	    { ".lua" },
	},
	{
	    ResourceFile::Type::None,
	    {},
	}
};

bool IsFileSupported(const String& extension, ResourceFile::Type supportedFileType);

/// Factory for ResourceFile objects. Implements creating, loading and saving files.                                \n
/// Maintains an internal cache that doesn't let the same file to be loaded twice. Cache misses force file loading. \n
/// This just means you can load the same file multiple times without worrying about unnecessary copies.            \n
/// All path arguments should be relative to Rootex root.
class ResourceLoader
{
	static HashMap<Ptr<ResourceData>, Ptr<ResourceFile>> s_ResourcesDataFiles;
	
	static void UpdateFileTimes(ResourceFile* file);
	static void LoadAssimp(ModelResourceFile* file);
	static void LoadALUT(AudioResourceFile* audioRes, const char* audioBuffer, int format, int size, float frequency);


public:
	static void RegisterAPI(sol::state& rootex);

	static TextResourceFile* CreateTextResourceFile(const String& path);
	static TextResourceFile* CreateNewTextResourceFile(const String& path);
	static LuaTextResourceFile* CreateLuaTextResourceFile(const String& path);
	static AudioResourceFile* CreateAudioResourceFile(const String& path);
	static ModelResourceFile* CreateModelResourceFile(const String& path);
	static ImageResourceFile* CreateImageResourceFile(const String& path);
	static FontResourceFile* CreateFontResourceFile(const String& path);
	
	/// Use when you don't know what kind of a resource file will it be
	static ResourceFile* CreateSomeResourceFile(const String& path);
	
	/// Write the data buffer inside a ResourceFile to disk.
	static void SaveResourceFile(ResourceFile* resourceFile);
	/// Reload the data buffer inside a ResourceFile from disk.
	static void ReloadResourceData(const String& path);

	static void Reload(TextResourceFile* file);
	static void Reload(LuaTextResourceFile* file);
	static void Reload(AudioResourceFile* file);
	static void Reload(ModelResourceFile* file);
	static void Reload(ImageResourceFile* file);
	static void Reload(FontResourceFile* file);

	/// Load all the files passed in, in a parellel manner. Return total tasks generated.
	static int Preload(Vector<String> paths, Atomic<int>& progress);
};
