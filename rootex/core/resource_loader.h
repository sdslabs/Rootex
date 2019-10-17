#pragma once

#include "common/types.h"
#include "resource.h"
#include "os/os.h"

class ResourceLoader
{
	static String findData(DirectoryShortcut directory, String path);

public:
	static TextFile* createFileResource(DirectoryShortcut directory, String path);
	static LuaScript* createScriptResource(DirectoryShortcut directory, String path);
};
