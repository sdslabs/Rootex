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
		Vector<Scene*> foundScenes;
		if (SceneLoader::GetSingleton()->getCurrentScene() && !(Scene::FindScenesByName(foundScenes,root,"PauseScene").size()))
		{
			Ptr<Scene>& pauseScene = Scene::CreateFromFile("rootex/assets/scenes/pause.scene.json");
			pauseScene->setName("PauseScene");
			root->addChild(pauseScene);
		}
	}
	else
	{
		Vector<Scene*> foundScenes;
		Vector<Scene*> pauseScenes = Scene::FindScenesByName(foundScenes,root,"PauseScene");
		for (auto& pauseScene : pauseScenes)
		{
			root->removeChild(pauseScene);
		}
	}
}
