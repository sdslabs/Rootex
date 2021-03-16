#include "script_system.h"

#include "script/script.h"
#include "framework/scene_loader.h"

ScriptSystem::ScriptSystem()
    : System("ScriptSystem", UpdateOrder::Update, true)
{
}

ScriptSystem* ScriptSystem::GetSingleton()
{
	static ScriptSystem singleton;
	return &singleton;
}

void ScriptSystem::addInitScriptEntity(Entity* e)
{
	m_ScriptEntitiesToInit.push_back(e);
}

void CallUpdateForScene(Scene* scene, float deltaMilliseconds)
{
	if (Entity* entity = scene->getEntity())
	{
		entity->call("update", { entity, deltaMilliseconds });
	}

	for (auto& child : scene->getChildren())
	{
		CallUpdateForScene(child.get(), deltaMilliseconds);
	}
}

void ScriptSystem::update(float deltaMilliseconds)
{
	ZoneScoped;

	for (auto& entity : m_ScriptEntitiesToInit)
	{
		if (entity)
		{
			entity->evaluateScriptOverrides();
			entity->call("begin", { entity });
		}
	}
	m_ScriptEntitiesToInit.clear();

	Scene* root = SceneLoader::GetSingleton()->getRootScene();
	CallUpdateForScene(root, deltaMilliseconds);
}

void CallDestroyForScene(Scene* scene)
{
	if (Entity* entity = scene->getEntity())
	{
		entity->call("destroy", { entity });
	}

	for (auto& child : scene->getChildren())
	{
		CallDestroyForScene(child.get());
	}
}

void ScriptSystem::end()
{
	Scene* root = SceneLoader::GetSingleton()->getRootScene();
	CallDestroyForScene(root);
}
