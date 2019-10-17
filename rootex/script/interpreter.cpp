#include "interpreter.h"

LuaInterpreter::LuaInterpreter()
{
	m_LuaState = luaL_newstate();
	luaL_openlibs(m_LuaState);
}

LuaInterpreter::~LuaInterpreter()
{
	lua_close(m_LuaState);
}

void LuaInterpreter::loadExecuteScript(LuaScript* script)
{
	luaL_dostring(m_LuaState, script->getContents().c_str());
	printLine(script->getPath() + " was run");
}

void LuaInterpreter::loadExecuteScript(const String& script)
{
	PANIC(script == "", "Lua inline script was found empty");
	luaL_dostring(m_LuaState, script.c_str());
}

luabridge::LuaRef LuaInterpreter::getGlobal(const String& name)
{
	luabridge::LuaRef result = luabridge::getGlobal(m_LuaState, name.c_str());
	if (result.isNil())
	{
		ERR("Lua variable (" + name + ") was not found");
	}
	return result;
}
