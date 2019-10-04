#pragma once

#include <string>
#include "resource.h"
#include "os/os.h"

class ResourceLoader
{
public:
	static TextFile* createFileResource(DirectoryShortcut directory, std::string path);
	static Script* createScriptResource(DirectoryShortcut directory, std::string path);
};
