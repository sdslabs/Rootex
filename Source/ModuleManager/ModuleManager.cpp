#include "ModuleManager.h"

Module *ModuleManager::CreateModule()
{
	Module *NewModule = new Module();
	m_Modules.push_back(new Module);

	return NewModule;
}

void ModuleManager::RegisterModule(Module *p_Module)
{
	m_Modules.push_back(p_Module);
}
