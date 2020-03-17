#pragma once

#include "common/types.h"
#include "core/resource_data.h"
#include "core/resource_file.h"
#include "os/os.h"

#include "vendor/OBJLoader/Source/OBJ_Loader.h"

class ResourceLoader
{
	static objl::Loader s_ModelLoader;
	static HashMap<Ptr<ResourceData>, Ptr<ResourceFile>> s_ResourcesDataFiles;
	static HashMap<ResourceFile::Type, Vector<ResourceFile*>> s_ResourceFileLibrary;

public:
	static TextResourceFile* CreateTextResourceFile(const String& path);
	static TextResourceFile* CreateNewTextResourceFile(const String& path);
	static LuaTextResourceFile* CreateLuaTextResourceFile(const String& path);
	static AudioResourceFile* CreateAudioResourceFile(const String& path);
	static VisualModelResourceFile* CreateVisualModelResourceFile(const String& path);
	static ImageResourceFile* CreateImageResourceFile(const String& path);
	static FontResourceFile* CreateFontResourceFile(const String& path, const String& name);

	static void SaveResourceFile(TextResourceFile*& resourceFile);
	static void ReloadResourceData(const String& path);

	static objl::Loader& GetModelLoader() { return s_ModelLoader; }
	static Vector<ResourceFile*>& GetFilesOfType(ResourceFile::Type type);
};
