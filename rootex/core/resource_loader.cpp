#include "resource_loader.h"

#include "common/common.h"
#include "resource_manager.h"

TextFile* ResourceLoader::createFileResource(DirectoryShortcut directory, std::string path)
{
	if (path == "")
	{
		return nullptr;
	}

	std::string r = OS::getSingleton().loadFileContents(directory, path);

	TextFile* res = new TextFile(path, r);

	return res;
}

Script* ResourceLoader::createScriptResource(DirectoryShortcut directory, std::string path)
{
	if (path == "")
	{
		return nullptr;
	}

	std::string r = OS::getSingleton().loadFileContents(directory, path);

	Script* res = new Script(path, r);

	return res;
}