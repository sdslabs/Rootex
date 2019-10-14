<<<<<<< HEAD
/*
#include <common.h>
=======
#include <string>

#include "common/common.h"
>>>>>>> master

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
	
	while (1)
	{
		Script* luaScript = ResourceLoader::createScriptResource(DirectoryShortcut::GAME, "assets\\config\\test.lua");
		inter.loadExecuteScript(luaScript);

		printLine(std::to_string((int)inter.getGlobal("test")));
	
		luabridge::LuaRef window = inter.getGlobal("window");
		printLine(window["width"]);
		printLine(window["height"]);
		printLine(window["type"]);

		std::cin.get();
	}
	ResourceManager::emptyAll();
    return 0;
}
*/
#include <iostream>
#include <core/renderer/window.h>
int main()
{
	RootexWindow* m_Window = new RootexWindow(100, 200, 640, 480);
	int ret = m_Window->gameLoop();
	delete m_Window;
	return ret;
}