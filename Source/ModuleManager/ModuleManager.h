#pragma once

#include <vector>

#include "Module.h"

class ModuleManager
{
private:
	std::vector<Module *> m_Modules;

	ModuleManager();
	~ModuleManager();

public:
	Module *CreateModule();
	void RegisterModule(Module *p_Module);

protected:
};
