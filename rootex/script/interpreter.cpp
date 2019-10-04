#include "interpreter.h"

Interpreter::Interpreter()
{
	m_LuaState = luaL_newstate();
	luaL_openlibs(m_LuaState);
}

Interpreter::~Interpreter()
{
	lua_close(m_LuaState);
}

void Interpreter::loadExecuteScript(Script* script)
{
	luaL_dostring(m_LuaState, script->getContents().c_str());
	print(script->getPath() + " was run");
}

void Interpreter::loadExecuteScript(const std::string& script)
{
	PANIC(script == "", "Lua inline script was found empty");
	luaL_dostring(m_LuaState, script.c_str());
}
