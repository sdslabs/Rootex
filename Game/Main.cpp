#include <common.h>

#include "core/resource_loader.h"
#include "os/os.h"
#include "script/lua_interpreter.h"

int main()
{
	printLine("Rootex Engine is starting: Build(" + std::string(__DATE__) + "|" + std::string(__TIME__) + ")");
    
	ResourceLoader rl;
	
	// Engine starts from build/game/.
	TextFile* r = rl.createFileResource("abc.txt"); // So this loads build/game/abc.txt (However the binary exists in build/game/Debug/)
	printLine(r->getContents());

	std::cin.get();

	char buff[256];
	int error;

	lua_State* L = luaL_newstate();
	luaopen_base(L);
	
	while (fgets(buff, sizeof(buff), stdin) != NULL)
	{
		error = luaL_loadbuffer(L, buff, strlen(buff), "line") || lua_pcall(L, 0, 0, 0);
		if (error)
		{
			fprintf(stderr, "%s", lua_tostring(L, -1));
			lua_pop(L, 1); /* pop error message from the stack */
		}
	}

    return 0;
}
