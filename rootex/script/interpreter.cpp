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
	printLine(script->getPath() + " was run");
}

void Interpreter::loadExecuteScript(const std::string& script)
{
	PANIC(script == "", "Lua inline script was found empty");
	luaL_dostring(m_LuaState, script.c_str());
}

luabridge::LuaRef Interpreter::getGlobal(const std::string& name)
{
	luabridge::LuaRef result = luabridge::getGlobal(m_LuaState, name.c_str());
	if (result.isNil())
	{
		ERR("Lua variable (" + name + ") was not found");
	}
	return result;
}
