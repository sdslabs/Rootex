#pragma once

#include "common/common.h"
#include "core/resource_file.h"

#include "vendor/Lua/src/lua.hpp"
#include "vendor/LuaBridge/Source/LuaBridge/LuaBridge.h"

typedef luabridge::LuaRef LuaVariable;

class LuaInterpreter
{
	lua_State* m_LuaState;

public:
	LuaInterpreter();
	~LuaInterpreter();

	void loadExecuteScript(Ref<ResourceFile> script);
	void loadExecuteScript(const String& script);

	LuaVariable getGlobal(const String& name);

protected:
};
