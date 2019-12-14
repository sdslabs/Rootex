#pragma once

#include "common/types.h"
#include "core/resource_file.h"
#include "core/resource_data.h"
#include "os/os.h"

class ResourceLoader
{
	static HashMap<Ptr<ResourceData>, Ptr<ResourceFile>> s_ResourcesDataFiles;

public:
	static TextResourceFile* createTextResourceFile(String path);
	static LuaTextResourceFile* createLuaTextResourceFile(String path);
	static AudioResourceFile* createAudioResourceFile(String path);
};
