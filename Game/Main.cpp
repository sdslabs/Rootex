#include <common.h>

#include "core/resource_loader.h"
#include "os/os.h"
#include "script/interpreter.h"

int main()
{
	printLine("Rootex Engine is starting: Build(" + std::string(__DATE__) + "|" + std::string(__TIME__) + ")");
    
	ResourceLoader rl;
	
	// Engine starts from build/game/.
	TextFile* r = rl.createFileResource("abc.txt"); // So this loads build/game/abc.txt (However the binary exists in build/game/Debug/)
	printLine(r->getContents());

	std::cin.get();

	lua_State* L = luaL_newstate();

	luaL_dostring(L, "x = 42");
	
	lua_pushnumber(L, 42);
	lua_pushnumber(L, 52);
	lua_pushnumber(L, 62);

	lua_close(L);

    return 0;
}
