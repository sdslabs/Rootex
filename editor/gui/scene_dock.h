#pragma once

#include "scene.h"

class SceneDock
{
public:
	struct SceneDockSettings
	{
		bool m_IsActive = true;
		bool m_IsEntitiesDockActive = true;
	};

private:
	SceneDockSettings m_SceneDockSettings;
	SceneID m_OpenedSceneID;

	void showSceneTree(Scene* scene);
	void openScene(Scene* scene);

	Variant selectOpenScene(const Event* event);

public:
	SceneDock();
	SceneDock(const SceneDock&) = delete;
	~SceneDock() = default;

	void draw(float deltaMilliseconds);

	void showEntities(Scene* scene);

	SceneDockSettings& getSettings() { return m_SceneDockSettings; }
	void setActive(bool enabled) { m_SceneDockSettings.m_IsActive = enabled; }
};
