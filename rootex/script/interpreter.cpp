#include "interpreter.h"
#include "core/resource_data.h"

extern "C"
{
	int luaopen_Rootex(lua_State* L);
}

#define LUA_EXTRALIBS { "Rootex", luaopen_Rootex },

LuaInterpreter::LuaInterpreter()
{
	m_LuaState = luaL_newstate();
	luaopen_base(m_LuaState);
	luaopen_Rootex(m_LuaState);
	luaL_openlibs(m_LuaState);
}

LuaInterpreter::~LuaInterpreter()
{
	lua_close(m_LuaState);
}

void LuaInterpreter::loadExecuteScript(LuaTextResourceFile* script)
{
	PANIC(script->getType() != ResourceFile::Type::LUA, "LuaInterpreter: Running non-Lua script source");

	luaL_dostring(m_LuaState, script->getString().c_str());
	OS::PrintLine(script->getPath().generic_string() + " was run");
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
