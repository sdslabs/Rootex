#pragma once

#include "common/common.h"
#include "core/resource.h"

#include "vendor/Lua/src/lua.hpp"
#include "vendor/LuaBridge/Source/LuaBridge/LuaBridge.h"

typedef luabridge::LuaRef LuaVariable;

class LuaInterpreter
{
	lua_State* m_LuaState;

public:
	LuaInterpreter();
	~LuaInterpreter();

	void loadExecuteScript(LuaScript* script);
	void loadExecuteScript(const String& script);

	luabridge::LuaRef getGlobal(const String& name);

protected:
};
