#include "script_system.h"

#include "script/script.h"
#include "framework/scene_loader.h"

ScriptSystem::ScriptSystem()
    : System("ScriptSystem", UpdateOrder::Update, true)
    , m_FluxTweener(LuaInterpreter::GetSingleton()->getLuaState().require_file("flux", "rootex/vendor/Flux/flux.lua"))
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

void ScriptSystem::removeInitScriptEntity(Entity* e)
{
	for (int i = 0; i < m_ScriptEntitiesToInit.size(); i++)
	{
		if (e == m_ScriptEntitiesToInit[i])
		{
			m_ScriptEntitiesToInit.erase(m_ScriptEntitiesToInit.begin() + i);
			return;
		}
	}
}

void ScriptSystem::addEnterScriptEntity(Entity* e)
{
	m_ScriptEntitiesToEnter.push_back(e);
}

void ScriptSystem::removeEnterScriptEntity(Entity* e)
{
	for (int i = 0; i < m_ScriptEntitiesToEnter.size(); i++)
	{
		if (e == m_ScriptEntitiesToEnter[i])
		{
			m_ScriptEntitiesToEnter.erase(m_ScriptEntitiesToEnter.begin() + i);
			return;
		}
	}
}

void CallUpdateForScene(Scene* scene, float deltaMilliseconds)
{
	scene->getEntity().call("update", { &scene->getEntity(), deltaMilliseconds });

	for (auto& child : scene->getChildren())
	{
		CallUpdateForScene(child.get(), deltaMilliseconds);
	}
}

void ScriptSystem::update(float deltaMilliseconds)
{
	ZoneScoped;
	if (!(m_Pause))
	{
		for (auto& entity : m_ScriptEntitiesToInit)
		{
			if (entity)
			{
				entity->evaluateScriptOverrides();
				entity->call("begin", { entity });
			}
		}
		m_ScriptEntitiesToInit.clear();

		for (auto& entity : m_ScriptEntitiesToEnter)
		{
			if (entity)
			{
				entity->call("enterScene", { entity });
			}
		}
		m_ScriptEntitiesToEnter.clear();

		Scene* root = SceneLoader::GetSingleton()->getRootScene();
		CallUpdateForScene(root, deltaMilliseconds);

		m_FluxTweener["update"](deltaMilliseconds * MS_TO_S);
	}
	else
	{
		for (auto& entity : m_ScriptEntitiesToInit)
		{
			if (entity && (!(entity->getScene()->m_ScenePause)))
			{
				entity->evaluateScriptOverrides();
				entity->call("begin", { entity });
			}
		}
		m_ScriptEntitiesToInit.clear();

		for (auto& entity : m_ScriptEntitiesToEnter)
		{
			if (entity && (!(entity->getScene()->m_ScenePause)))
			{
				entity->call("enterScene", { entity });
			}
		}
		m_ScriptEntitiesToEnter.clear();

		Scene* root = SceneLoader::GetSingleton()->getRootScene();
		CallUpdateForScene(root, deltaMilliseconds);

		m_FluxTweener["update"](deltaMilliseconds * MS_TO_S);
	}
}

void CallDestroyForScene(Scene* scene)
{
	scene->getEntity().call("destroy", { &scene->getEntity() });

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
