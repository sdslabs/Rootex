#pragma once

#include "common/common.h"
#include "event_manager.h"

class Scene;

class SceneLoader
{
	Ref<Scene> m_CurrentScene;
	Ref<Scene> m_RootScene;

	Vector<String> m_UnloadCache;
	Vector<String> m_SceneArguments;

	SceneLoader();
	
	void beginSystems();
	void endSystems();

	void setArguments(const Vector<String>& arguments) { m_SceneArguments = arguments; }
	Vector<String> getArguments() { return m_SceneArguments; }

	Variant deleteScene(const Event* event);

public:
	static void RegisterAPI(sol::table& rootex);
	static SceneLoader* GetSingleton();

	int preloadScene(const String& sceneFile, Atomic<int>& progress);
	void loadPreloadedScene(const String& sceneFile, const Vector<String>& arguments);
	void loadScene(const String& sceneFile, const Vector<String>& arguments);
	bool saveScene(Scene* scene);
	bool saveSceneAtFile(Scene* scene, const String& filePath);

	Ref<Scene> getCurrentScene() const { return m_CurrentScene; }
	Ref<Scene> getRootScene() const { return m_RootScene; }
};
