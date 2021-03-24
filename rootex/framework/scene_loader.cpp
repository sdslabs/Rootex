#include "scene_loader.h"

#include "resource_loader.h"
#include "event_manager.h"
#include "scene.h"
#include "system.h"

SceneLoader::SceneLoader()
    : m_RootScene(Scene::CreateRootScene())
{
	BIND_EVENT_MEMBER_FUNCTION(RootexEvents::DeleteScene, deleteScene);
}

SceneLoader* SceneLoader::GetSingleton()
{
	static SceneLoader singleton;
	return &singleton;
}

Vector<String> SceneLoader::findResourcePaths(const JSON::json& sceneJSON)
{
	Vector<String> resources;
	JSON::json flat = sceneJSON.flatten();

	for (auto& [key, value] : flat.items())
	{
		if (value.is_string())
		{
			String assetPath = (String)value;
			if (assetPath.find("assets/") != String::npos)
			{
				resources.push_back(assetPath);
			}
		}
	}
	return resources;
}

Variant SceneLoader::deleteScene(const Event* event)
{
	Scene* scene = Extract<Scene*>(event->getData());
	if (Scene* parent = scene->getParent())
	{
		parent->removeChild(scene);
	}
	else
	{
		ERR("Cannot force delete an orphan scene. This is a serious bug. Orphan scenes shouldn't exist at all. Or you tried to delete the Root scene.")
	}
	return true;
}

int SceneLoader::preloadScene(const String& sceneFile, Atomic<int>& progress)
{
	const SceneSettings& sceneJSON = JSON::json::parse(ResourceLoader::CreateTextResourceFile(sceneFile)->getString()).value("settings", SceneSettings());
	return ResourceLoader::Preload(sceneJSON.preloads, progress);
}

void SceneLoader::loadPreloadedScene(const String& sceneFile, const Vector<String>& arguments)
{
	EventManager::GetSingleton()->defer([this, sceneFile, arguments]() {
		endSystems();
		m_RootScene->removeChild(m_CurrentScene);
		Scene::ResetNextID();

		ResourceLoader::ClearDeadResources();
		Ref<TextResourceFile> sceneResFile = ResourceLoader::CreateTextResourceFile(sceneFile);
		if (sceneResFile->isDirty())
		{
			sceneResFile->reimport();
		}
		Ptr<Scene>& scene = Scene::Create(JSON::json::parse(sceneResFile->getString()));
		m_CurrentScene = scene.get();
		m_RootScene->addChild(scene);
		setArguments(arguments);

		for (auto& systems : System::GetSystems())
		{
			for (auto& system : systems)
			{
				system->setConfig(m_CurrentScene->getSettings());
			}
		}
		m_CurrentScene->onLoad();
		PRINT("Loaded scene: " + m_CurrentScene->getFullName());

		ResourceLoader::ClearPersistentResources();

		beginSystems();
		EventManager::GetSingleton()->deferredCall(RootexEvents::OpenedScene);
	});
}

void SceneLoader::loadScene(const String& sceneFile, const Vector<String>& arguments)
{
	Atomic<int> progress;
	int totalProgress = preloadScene(sceneFile, progress);

	PRINT("Waiting for " + std::to_string(totalProgress) + " resources to load...");
	while (progress.load() != totalProgress)
	{
		;
	}
	PRINT("Preloaded scene file (" + std::to_string(totalProgress) + " new resources)");

	loadPreloadedScene(sceneFile, arguments);
}

bool SceneLoader::saveScene(Scene* scene)
{
	return saveSceneAtFile(scene, scene->getScenePath());
}

bool SceneLoader::saveSceneAtFile(Scene* scene, const String& filePath)
{
	if (Ref<TextResourceFile> file = ResourceLoader::CreateNewTextResourceFile(filePath))
	{
		file->putString(scene->getJSON().dump(4));
		return file->save();
	}
	return false;
}

void SceneLoader::destroyAllScenes()
{
	m_CurrentScene = nullptr;
	m_RootScene.reset();
}

void SceneLoader::beginSystems()
{
	if (m_CurrentScene)
	{
		for (auto& systems : System::GetSystems())
		{
			for (auto& system : systems)
			{
				if (system->isActive())
				{
					system->begin();
				}
			}
		}
	}
}

void SceneLoader::endSystems()
{
	if (m_CurrentScene)
	{
		for (auto& systems : System::GetSystems())
		{
			for (auto& system : systems)
			{
				system->end();
			}
		}
	}
}
