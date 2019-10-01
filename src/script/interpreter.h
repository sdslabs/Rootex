#pragma once

#include "common.h"
#include "core/resource.h"

extern "C"
{
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}
#include "LuaBridge/LuaBridge.h"

class Interpreter
{

public:
	Interpreter();
	~Interpreter();

	void loadExecuteScript(Script& script);
	void loadExecuteScript(const std::string& script);

protected:
};
