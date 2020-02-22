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
	LuaInterpreter(LuaInterpreter&) = delete;
	~LuaInterpreter();

public:
	static LuaInterpreter* GetSingleton();

	void loadExecuteScript(LuaTextResourceFile* script);
	void loadExecuteScript(const String& script);

	lua_State* getState() const { return m_LuaState; }
	LuaVariable getGlobal(const String& name);

protected:
};
