#include "resource_loader.h"

#include "common/common.h"
#include "resource_manager.h"

void ResourceLoader::assign(IResourceFile* file, ResourceData* resource)
{
	file->m_ResourceData = resource;
}

void ResourceLoader::loadDataInResourceFile(DirectoryShortcut directory, String& path, IResourceFile* res)
{
	FileBuffer buffer = OS::getSingleton().loadFileContents(directory, path);
	ResourceData* resData = new ResourceData(buffer.m_Buffer);

	resData->m_Path = res->getPath();
	assign(res, resData);
}

Ref<TextResource> ResourceLoader::createTextResourceFile(DirectoryShortcut directory, String name, String path)
{
	TextResource* res = new TextResource(name, OS::getSingleton().getAbsolutePath(directory, path).generic_string());

	loadDataInResourceFile(directory, path, res);

	return Ref<TextResource>(res);
}

Ref<LuaScriptResource> ResourceLoader::createLuaScriptResourceFile(DirectoryShortcut directory, String name, String path)
{
	LuaScriptResource* res = new LuaScriptResource(name, OS::getSingleton().getAbsolutePath(directory, path).generic_string());

	loadDataInResourceFile(directory, path, res);

	return Ref<LuaScriptResource>(res);
}

Ref<WAVResource> ResourceLoader::createWAVResourceFile(DirectoryShortcut directory, String name, String path)
{
	WAVResource* res = new WAVResource(name, OS::getSingleton().getAbsolutePath(directory, path).generic_string());

	loadDataInResourceFile(directory, path, res);

	return Ref<WAVResource>(res);
}
