#pragma once

#include "common/common.h"
#include "event_manager.h"

class Scene;

class SceneLoader
{
	EventBinder<SceneLoader> m_Binder;

	Scene* m_CurrentScene = nullptr;
	Ptr<Scene> m_RootScene;

	Vector<String> m_SceneArguments;

	SceneLoader();

	void beginSystems();
	void endSystems();

	void setArguments(const Vector<String>& arguments) { m_SceneArguments = arguments; }
	Vector<String> findResourcePaths(const JSON::json& sceneJSON);

	Variant deleteScene(const Event* event);

public:
	static SceneLoader* GetSingleton();

	int preloadScene(const String& sceneFile, Atomic<int>& progress);
	void loadPreloadedScene(const String& sceneFile, const Vector<String>& arguments);
	void loadScene(const String& sceneFile, const Vector<String>& arguments);
	int exportScene(Scene* scene, Atomic<int>& progress);
	void postExport();
	bool saveScene(Scene* scene);
	bool saveSceneAtFile(Scene* scene, const String& filePath);
	void destroyAllScenes();

	Scene* getCurrentScene() const { return m_CurrentScene; }
	Scene* getRootScene() const { return m_RootScene.get(); }
	Ptr<Scene>& getRootSceneEx() { return m_RootScene; }
	Vector<String> getArguments() { return m_SceneArguments; }
};
