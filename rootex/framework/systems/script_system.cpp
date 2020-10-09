#include "script_system.h"

#include "components/script_component.h"

ScriptSystem::ScriptSystem()
    : System("ScriptSystem", UpdateOrder::Update, true)
{
}

ScriptSystem* ScriptSystem::GetSingleton()
{
	static ScriptSystem singleton;
	return &singleton;
}

void ScriptSystem::begin()
{
	for (auto&& [id, entity] : EntityFactory::GetSingleton()->getEntities())
	{
		entity->call("onBegin", { entity });
	}
}

void ScriptSystem::update(float deltaMilliseconds)
{
	for (auto&& [id, entity] : EntityFactory::GetSingleton()->getEntities())
	{
		entity->call("onUpdate", { entity, deltaMilliseconds });
	}
}

void ScriptSystem::end()
{
	for (auto&& [id, entity] : EntityFactory::GetSingleton()->getEntities())
	{
		entity->call("onEnd", { entity });
	}
}
