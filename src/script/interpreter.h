#pragma once

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "common.h"
#include "core/resource.h"

class Interpreter
{
	lua_State* m_LuaState;

	lua_State* init();

public:
	Interpreter();
	~Interpreter();

	void loadScript(Script& script);
	void runScript();

protected:
};
