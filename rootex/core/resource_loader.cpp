#include "resource_loader.h"

#include "common/common.h"
#include "resource_manager.h"

void ResourceLoader::assign(IResourceFile* file, ResourceData* resource)
{
	file->m_ResourceData = Ref<ResourceData>(resource);
}

template<class ResourceFile>
Ref<ResourceFile> ResourceLoader::createFileResource(DirectoryShortcut directory, String name, String path)
{
	ResourceFile* res = new ResourceFile(name, OS::getSingleton().getAbsolutePath(directory, path).generic_string());

	FileBuffer buffer = OS::getSingleton().loadFileContents(directory, path);
	ResourceData* resData = new ResourceData(buffer.m_Buffer, buffer.m_Size);

	resData->m_Path = res->getPath();
	assign(res, resData);

	return Ref<ResourceFile>(res);
}