#pragma once

#include "common/types.h"
#include "os/os.h"
#include "resource.h"

class ResourceLoader
{
	static String findData(DirectoryShortcut directory, String path);

public:
	static Ref<TextFile> createFileResource(DirectoryShortcut directory, String path);
	static Ref<LuaScriptResource> createScriptResource(DirectoryShortcut directory, String path);
};
