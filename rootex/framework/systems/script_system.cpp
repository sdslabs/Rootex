#include "script_system.h"

#include "components/script_component.h"

ScriptSystem* ScriptSystem::GetSingleton()
{
	static ScriptSystem singleton;
	return &singleton;
}

void ScriptSystem::begin()
{
	ScriptComponent* scriptComponent = nullptr;
	for (auto&& component : s_Components[ScriptComponent::s_ID])
	{
		try
		{
			scriptComponent = (ScriptComponent*)component;
			scriptComponent->onBegin();
		}
		catch (std::exception e)
		{
			ERR(e.what());
		}
	}
}

void ScriptSystem::update(float deltaMilliseconds)
{
	ScriptComponent* scriptComponent = nullptr;
	for (auto&& component : s_Components[ScriptComponent::s_ID])
	{
		try
		{
			scriptComponent = (ScriptComponent*)component;
			scriptComponent->onUpdate(deltaMilliseconds);
		}
		catch (std::exception e)
		{
			ERR(e.what());
		}
	}
}

void ScriptSystem::end()
{
	ScriptComponent* scriptComponent = nullptr;
	for (auto&& component : s_Components[ScriptComponent::s_ID])
	{
		try
		{
			scriptComponent = (ScriptComponent*)component;
			scriptComponent->onEnd();
		}
		catch (std::exception e)
		{
			ERR(e.what());
		}
	}
}
