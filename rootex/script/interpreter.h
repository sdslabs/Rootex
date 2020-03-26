#pragma once

#include "common/common.h"
#include "core/resource_file.h"

#include "vendor/Lua/src/lua.hpp"
#define SOL_ALL_SAFETIES_ON	1
#define SOL_USING_CXX_LUA 1
#define SOL_PRINT_ERRORS 1
#include "sol/sol.hpp"

class LuaInterpreter
{
	sol::state m_Lua;
	
	LuaInterpreter();
	LuaInterpreter(LuaInterpreter&) = delete;
	~LuaInterpreter() = default;

	void registerTypes();

public:
	static LuaInterpreter* GetSingleton();

	sol::state& getLuaState() { return m_Lua; }
};
