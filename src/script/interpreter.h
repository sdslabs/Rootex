#pragma once

#include "common.h"
#include "core/resource.h"
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
