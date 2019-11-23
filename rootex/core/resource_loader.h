#pragma once

#include "common/types.h"
#include "core/resource_file.h"
#include "os/os.h"

namespace ResourceLoader
{
	void assign(ResourceFile* file, ResourceData* resource);
	void loadDataInResourceFile(DirectoryShortcut directory, String& path, ResourceFile* res);

	Ref<ResourceFile> createResourceFile(DirectoryShortcut directory, String name, String path, ResourceFile::Type type);
};
