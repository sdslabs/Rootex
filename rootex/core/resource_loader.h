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

public:
	static TextResourceFile* CreateTextResourceFile(String path);
	static LuaTextResourceFile* CreateLuaTextResourceFile(String path);
	static AudioResourceFile* CreateAudioResourceFile(String path);
	static VisualModelResourceFile* CreateVisualModelResourceFile(String path);
	static ImageResourceFile* CreateImageResourceFile(String path);

	static void SaveResourceFile(TextResourceFile*& resourceFile);
	static void ReloadResourceData(const String& path);

	static objl::Loader& GetModelLoader() { return s_ModelLoader; }
};
