#pragma once

#include "common/types.h"
#include "core/resource_file.h"
#include "os/os.h"

class ResourceLoader
{
	static void assign(IResourceFile* file, ResourceData* resource);
	static void loadDataInResourceFile(DirectoryShortcut directory, String& path, IResourceFile* res);

public:
	static Ref<TextResource> createTextResourceFile(DirectoryShortcut directory, String name, String path);
	static Ref<LuaScriptResource> createLuaScriptResourceFile(DirectoryShortcut directory, String name, String path);
	static Ref<WAVResource> createWAVResourceFile(DirectoryShortcut directory, String name, String path);
};
