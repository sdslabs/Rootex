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

void CallBeginForScene(Scene* scene)
{
	if (Entity* entity = scene->getEntity())
	{
		entity->evaluateScriptOverrides();
		entity->call("onBegin", { entity });
	}

	for (auto& child : scene->getChildren())
	{
		CallBeginForScene(child.get());
	}
}

void ScriptSystem::begin()
{
	Scene* root = SceneLoader::GetSingleton()->getRootScene();
	CallBeginForScene(root);
}

void CallUpdateForScene(Scene* scene, float deltaMilliseconds)
{
	if (Entity* entity = scene->getEntity())
	{
		entity->evaluateScriptOverrides();
		entity->call("onUpdate", { entity, deltaMilliseconds });
	}

	for (auto& child : scene->getChildren())
	{
		CallUpdateForScene(child.get(), deltaMilliseconds);
	}
}

void ScriptSystem::update(float deltaMilliseconds)
{
	ZoneScoped;
	Scene* root = SceneLoader::GetSingleton()->getRootScene();
	CallUpdateForScene(root, deltaMilliseconds);
}

void CallEndForScene(Scene* scene)
{
	if (Entity* entity = scene->getEntity())
	{
		entity->evaluateScriptOverrides();
		entity->call("onEnd", { entity });
	}

	for (auto& child : scene->getChildren())
	{
		CallEndForScene(child.get());
	}
}

void ScriptSystem::end()
{
	Scene* root = SceneLoader::GetSingleton()->getRootScene();
	CallEndForScene(root);
}
