/*
#include <common.h>

#include "core/resource_loader.h"
#include "os/os.h"

int main()
{
	printLine("Rootex Engine is starting: Build(" + std::string(__DATE__) + "|" + std::string(__TIME__) + ")");
    
	ResourceLoader rl;
	
	// Engine starts from build/game/.
	TextFile* r = rl.createFileResource("abc.txt"); // So this loads build/game/abc.txt (However the binary exists in build/game/Debug/)
	printLine(r->getContents());

	std::cin.get();

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