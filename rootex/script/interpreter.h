#pragma once

#include "common/common.h"
#include "core/resource.h"

#include "vendor/Lua/src/lua.hpp"
#include "vendor/LuaBridge/Source/LuaBridge/LuaBridge.h"

typedef luabridge::LuaRef LuaVariable;

class Interpreter
{
	lua_State* m_LuaState;

public:
	Interpreter();
	~Interpreter();

	void loadExecuteScript(Script* script);
	void loadExecuteScript(const std::string& script);

	luabridge::LuaRef getGlobal(const std::string& name);

protected:
};
