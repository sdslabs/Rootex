#include "resource_loader.h"

#include "common/common.h"
#include "resource_data_reserve.h"

namespace ResourceLoader
{
void assign(ResourceFile* file, ResourceData* resource)
{
	file->setResourceData(resource);
}

void loadDataInResourceFile(DirectoryShortcut directory, String& path, ResourceFile* res)
{
	FileBuffer buffer = OS::loadFileContents(directory, path);
	ResourceData* resData = new ResourceData(buffer.m_Buffer);

	resData->setPath(res->getPath());
	assign(res, resData);
}

Ref<ResourceFile> createResourceFile(DirectoryShortcut directory, String name, String path, ResourceFile::Type type)
{
	ResourceFile* res = new ResourceFile(name, OS::getAbsolutePath(directory, path).generic_string(), type);

	loadDataInResourceFile(directory, path, res);

	return Ref<ResourceFile>(res);
}
}
