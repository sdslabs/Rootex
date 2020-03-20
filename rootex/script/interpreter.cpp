#include "interpreter.h"
#include "core/resource_loader.h"

extern "C"
{
	int luaopen_Rootex(lua_State* L);
}

#define LUA_EXTRALIBS { "Rootex", luaopen_Rootex },

LuaInterpreter::LuaInterpreter()
{
	m_LuaState = luaL_newstate();
	luaopen_base(m_LuaState);
	luaL_openlibs(m_LuaState);
	bool error = luaopen_Rootex(m_LuaState);
	if (!error)
	{
		ERR("Error occured in loading Rootex Lua bindings: " + String(lua_tostring(m_LuaState, -1)));
	}

	loadExecuteScript(ResourceLoader::CreateLuaTextResourceFile("rootex/script/script_env.lua"));
}

LuaInterpreter::~LuaInterpreter()
{
	lua_close(m_LuaState);
}

LuaInterpreter* LuaInterpreter::GetSingleton()
{
	static LuaInterpreter singleton;
	return &singleton;
}

void LuaInterpreter::loadExecuteScript(LuaTextResourceFile* script)
{
	PANIC(script->getType() != ResourceFile::Type::Lua, "LuaInterpreter: Running non-Lua script source");

	bool error = luaL_dostring(m_LuaState, script->getString().c_str());
	OS::PrintLine(script->getPath().generic_string() + " was run");
	if (error)
	{
		ERR("Error occured in " + script->getPath().generic_string() + ": " + String(lua_tostring(m_LuaState, -1)));
	}
}

void LuaInterpreter::loadExecuteScript(const String& script)
{
	PANIC(script == "", "Lua inline script was found empty");
	bool error = luaL_dostring(m_LuaState, script.c_str());
	if (error)
	{
		ERR("Error occured in string script:\n" + script + "\nError: " + String(lua_tostring(m_LuaState, -1)));
	}
}

LuaVariable LuaInterpreter::createTable(const String& name)
{
	return luabridge::newTable(m_LuaState);
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

void LuaInterpreter::setGlobal(LuaVariable luaVar, const String& name)
{
	luabridge::setGlobal(m_LuaState, luaVar, name.c_str());
}
