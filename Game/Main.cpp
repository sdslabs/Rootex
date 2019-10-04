#include "common/common.h"

#include "core/resource_loader.h"
#include "core/resource_manager.h"
#include "os/os.h"
#include "script/interpreter.h"

int main()
{
	printLine("Rootex Engine is starting: Build(" + std::string(__DATE__) + "|" + std::string(__TIME__) + ")");
    
	// Engine starts from build/game/.
	TextFile* r = ResourceLoader::createFileResource(DirectoryShortcut::ENGINE, "test\\abc.txt"); // So this loads build/game/abc.txt (However the binary exists in build/game/Debug/)
	printLine(r->getContents());

	Interpreter inter;
	Script* luaScript = ResourceLoader::createScriptResource(DirectoryShortcut::GAME, "assets\\config\\test.lua");
	inter.loadExecuteScript(luaScript);

	std::cin.get();
	ResourceManager::emptyAll();
    return 0;
}
