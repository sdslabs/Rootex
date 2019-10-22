#include "resource_loader.h"

#include "common/common.h"
#include "resource_manager.h"

String ResourceLoader::findData(DirectoryShortcut directory, String path)
{
	if (path == "")
	{
		WARN(String("File path in ").append(std::to_string((int)directory)).append(" was an empty string"));
		return "";
	}

	String r = OS::getSingleton().loadFileContents(directory, path);
	return r;
}

TextFile* ResourceLoader::createFileResource(DirectoryShortcut directory, String path)
{
	TextFile* res = new TextFile(path, findData(directory, path));

	return res;
}

LuaScriptResource* ResourceLoader::createScriptResource(DirectoryShortcut directory, String path)
{
	LuaScriptResource* res = new LuaScriptResource(path, findData(directory, path));

	return res;
}