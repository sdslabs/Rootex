#pragma once

#include "common/common.h"
#include "core/resource.h"

extern "C"
{
#include "vendor/Lua/src/lauxlib.h"
#include "vendor/Lua/src/lua.h"
#include "vendor/Lua/src/lualib.h"
}
#include "vendor/LuaBridge/Source/LuaBridge/LuaBridge.h"

class Interpreter
{

public:
	Interpreter();
	~Interpreter();

	void loadExecuteScript(Script& script);
	void loadExecuteScript(const std::string& script);

protected:
};
