#include "interpreter.h"
#include "core/resource_data.h"

LuaInterpreter::LuaInterpreter()
{
	m_LuaState = luaL_newstate();
	luaL_openlibs(m_LuaState);
}

LuaInterpreter::~LuaInterpreter()
{
	lua_close(m_LuaState);
}

void LuaInterpreter::loadExecuteScript(Ref<ResourceFile> script)
{
	PANIC(script->getType() != ResourceFile::Type::LUA, "LuaInterpreter: Running non-Lua scrip source");

	luaL_dostring(m_LuaState, script->getDataString().c_str());
	OS::printLine(script->getPath() + " was run");
}

void LuaInterpreter::loadExecuteScript(const String& script)
{
	PANIC(script == "", "Lua inline script was found empty");
	luaL_dostring(m_LuaState, script.c_str());
}

LuaVariable LuaInterpreter::getGlobal(const String& name)
{
	LuaVariable result = luabridge::getGlobal(m_LuaState, name.c_str());
	if (result.isNil())
	{
		ERR("Lua variable (" + name + ") was not found");
	}
	return result;
}
