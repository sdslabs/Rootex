#include "pause_system.h"

PauseSystem* PauseSystem::GetSingleton()
{
	static PauseSystem singleton;
	return &singleton;
}

PauseSystem::PauseSystem()
    : System("PauseSystem", UpdateOrder::Async, true)
{
	m_IsPausingEnabled = true;
}

void PauseSystem::update(float deltaMilliseconds)
{
	Scene* root = SceneLoader::GetSingleton()->getCurrentScene();
	if (m_IsPausingEnabled)
	{
		if (!(Scene::FindScenesByName("PauseScene").size()))
		{
			Ptr<Scene>& pauseScene = Scene::CreateFromFile("game/assets/scenes/pause.scene.json");
			pauseScene->setName("PauseScene");
			root->addChild(pauseScene);
		}
	}
	else
	{
		Vector<Scene*> pauseScenes = Scene::FindScenesByName("PauseScene");
		for (auto& pauseScene : pauseScenes)
		{
			root->removeChild(pauseScene);
		}
	}
}
