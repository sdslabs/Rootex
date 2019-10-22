#include "interpreter.h"

LuaInterpreter::LuaInterpreter()
{
	m_LuaState = Ptr<lua_State>(luaL_newstate());
	luaL_openlibs(m_LuaState.get());
}

LuaInterpreter::~LuaInterpreter()
{
	lua_close(m_LuaState.get());
}

void LuaInterpreter::loadExecuteScript(Ref<LuaScriptResource> script)
{
	luaL_dostring(m_LuaState.get(), script->getContents().c_str());
	printLine(script->getPath() + " was run");
}

void LuaInterpreter::loadExecuteScript(const String& script)
{
	PANIC(script == "", "Lua inline script was found empty");
	luaL_dostring(m_LuaState.get(), script.c_str());
}

luabridge::LuaRef LuaInterpreter::getGlobal(const String& name)
{
	luabridge::LuaRef result = luabridge::getGlobal(m_LuaState.get(), name.c_str());
	if (result.isNil())
	{
		ERR("Lua variable (" + name + ") was not found");
	}
	return result;
}
