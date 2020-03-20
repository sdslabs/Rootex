#pragma once

#include "common/common.h"
#include "core/resource_file.h"

#include "vendor/Lua/src/lua.hpp"
#include "vendor/LuaBridge/Source/LuaBridge/LuaBridge.h"

typedef luabridge::LuaRef LuaVariable;

class LuaInterpreter
{
	lua_State* m_LuaState;
	
	LuaInterpreter();
	~LuaInterpreter();

public:
	static LuaInterpreter* GetSingleton();

	void registerTypes();
	void loadExecuteScript(LuaTextResourceFile* script);
	void loadExecuteScript(const String& script);

	LuaVariable createTable(const String& name);

	LuaVariable getGlobal(const String& name);
	lua_State* getLuaState() const { return m_LuaState; }
	
	void setGlobal(LuaVariable luaVar, const String& name);
};
