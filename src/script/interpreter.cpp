#include "interpreter.h"

lua_State* Interpreter::init()
{
	return luaL_newstate();
}

Interpreter::Interpreter()
{
	m_LuaState = init();

	PANIC(m_LuaState == nullptr, "Lua state could not be created (set as null)");
}

Interpreter::~Interpreter()
{
	lua_close(m_LuaState);
}

void Interpreter::loadScript(Script& script)
{
}

void Interpreter::runScript()
{
}
