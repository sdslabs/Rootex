#pragma once

#include <string>
#include "resource.h"
#include "os/os.h"

class ResourceLoader
{
public:
	static TextFile* createFileResource(DirectoryShortcut directory, String path);
	static LuaScript* createScriptResource(DirectoryShortcut directory, String path);
};
