#pragma once

#include "common/common.h"
#include "core/resource_data.h"
#include "core/resource_file.h"
#include "os/os.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

static inline const Vector<String> SupportedImageExtensions = {
	".png",
	".jpeg",
	".jpg",
};

static inline const Vector<String> SupportedModelExtensions = {
	".obj"
};

static inline const Vector<String> SupportedAnimatedModelExtensions = {
	".dae"
};

static inline const Vector<String> SupportedAudioExtensions = {
	".wav"
};

bool IsSupported(const String& extension, const Vector<String>& supportedExtensions);

/// Factory for ResourceFile objects. Implements creating, loading and saving files.                                \n
/// Maintains an internal cache that doesn't let the same file to be loaded twice. Cache misses force file loading. \n
/// This just means you can load the same file multiple times without worrying about unnecessary copies.            \n
/// All path arguments should be relative to Rootex root.
class ResourceLoader
{
	static HashMap<Ptr<ResourceData>, Ptr<ResourceFile>> s_ResourcesDataFiles;
	static HashMap<ResourceFile::Type, Vector<ResourceFile*>> s_ResourceFileLibrary;

	static void UpdateFileTimes(ResourceFile* resourceFile);

public:
	static TextResourceFile* CreateTextResourceFile(const String& path);
	static TextResourceFile* CreateNewTextResourceFile(const String& path);
	static LuaTextResourceFile* CreateLuaTextResourceFile(const String& path);
	static AudioResourceFile* CreateAudioResourceFile(const String& path);
	static SkeletalAnimationResourceFile* CreateSkeletalAnimationResourceFile(const String& path);
	static VisualModelResourceFile* CreateVisualModelResourceFile(const String& path);
	static ImageResourceFile* CreateImageResourceFile(const String& path);
	static FontResourceFile* CreateFontResourceFile(const String& path);

	/// Write the data buffer inside a ResourceFile to disk.
	static void SaveResourceFile(ResourceFile* resourceFile);
	/// Reload the data buffer inside a ResourceFile from disk.
	static void ReloadResourceData(const String& path);

	/// Reload file from disk. Overwrites the data buffer after reloading file data from disk.
	static void Reload(TextResourceFile* textFile);
	static void Reload(AudioResourceFile* audioRes);
	static void Reload(SkeletalAnimationResourceFile* animFile);
	static void Reload(VisualModelResourceFile* modelFile);
	static void Reload(ImageResourceFile* imageFile);
	static void Reload(FontResourceFile* fontFile);

	/// Get a list of files that have already been loaded and belong to a certain type
	static Vector<ResourceFile*>& GetFilesOfType(ResourceFile::Type type);
	/// Get a list of all files that have already been loaded
	static HashMap<ResourceFile::Type, Vector<ResourceFile*>>& GetAllFiles();
};
