#pragma once

#include "common/types.h"
#include "core/resource_data.h"
#include "core/resource_file.h"
#include "os/os.h"

class ResourceLoader
{
	static HashMap<Ptr<ResourceData>, Ptr<ResourceFile>> s_ResourcesDataFiles;

public:
	static TextResourceFile* CreateTextResourceFile(String path);
	static LuaTextResourceFile* CreateLuaTextResourceFile(String path);
	static AudioResourceFile* CreateAudioResourceFile(String path);
	static VisualModelResourceFile* CreateVisualModelResourceFile(String path);
};
