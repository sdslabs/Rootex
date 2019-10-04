#include "resource_loader.h"

#include "common/common.h"
#include "os/os.h"
#include "resource_manager.h"

TextFile* ResourceLoader::createFileResource(std::string path)
{
	PANIC(path == "", "Resource path requested is empty");

	std::string r = OS::getSingleton().loadFileContents(path);

	TextFile* res = new TextFile(path, r);

	ResourceManager::registerResource(res);
	return res;
}
