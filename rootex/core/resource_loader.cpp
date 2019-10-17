#include "resource_loader.h"

#include "common/common.h"
#include "resource_manager.h"

TextFile* ResourceLoader::createFileResource(DirectoryShortcut directory, String path)
{
	if (path == "")
	{
		return nullptr;
	}

	String r = OS::getSingleton().loadFileContents(directory, path);

	TextFile* res = new TextFile(path, r);

	return res;
}

LuaScript* ResourceLoader::createScriptResource(DirectoryShortcut directory, String path)
{
	if (path == "")
	{
		return nullptr;
	}

	String r = OS::getSingleton().loadFileContents(directory, path);

	LuaScript* res = new LuaScript(path, r);

	return res;
}