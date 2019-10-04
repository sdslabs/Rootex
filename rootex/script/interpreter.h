#pragma once

#include "common/common.h"
#include "core/resource.h"

#include "vendor/Lua/src/lua.hpp"
#include "vendor/LuaBridge/Source/LuaBridge/LuaBridge.h"

class Interpreter
{
	lua_State* m_LuaState;

public:
	Interpreter();
	~Interpreter();

	void loadExecuteScript(Script* script);
	void loadExecuteScript(const std::string& script);

protected:
};
