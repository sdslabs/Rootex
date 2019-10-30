#pragma once

#include "common/types.h"
#include "os/os.h"
#include "resource_file.h"

class ResourceLoader
{
	static void assign(IResourceFile* file, ResourceData* resource);

public:
	template<class ResourceFile>
	static Ref<ResourceFile> createFileResource(DirectoryShortcut directory, String name, String path);
};
