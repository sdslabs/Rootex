#pragma once

#include "common/common.h"
#include "entity.h"

typedef unsigned int SceneID;

#define ROOT_SCENE_ID 1

struct SceneSettings
{
	Vector<String> preloads = {};
	SceneID camera = ROOT_SCENE_ID;
	SceneID listener = ROOT_SCENE_ID;
	JSON::json inputSchemes = {};
	String startScheme = {};

#ifdef ROOTEX_EDITOR
	void drawSceneSelectables(Scene* scene, SceneID& toSet);
	void draw();
#endif // ROOTEX_EDITOR
};

void to_json(JSON::json& j, const SceneSettings& s);
void from_json(const JSON::json& j, SceneSettings& s);

class Scene
{
	SceneID m_ID;
	String m_Name;
	String m_FullName;
	String m_SceneFile;
	Ref<Entity> m_Entity;
	SceneSettings m_Settings;

	Scene* m_ParentScene = nullptr;
	Vector<Ref<Scene>> m_ChildrenScenes;

public:
	static void RegisterAPI(sol::table& rootex);
	static void ResetCounter();

	static Ref<Scene> Create(const JSON::json& sceneData);
	static Ref<Scene> CreateFromFile(const String& sceneFile);
	static Ref<Scene> CreateEmpty();
	static Ref<Scene> CreateEmptyAtPath(const String& sceneFile);
	static Ref<Scene> CreateEmptyWithEntity();
	static Ref<Scene> CreateRootScene();

	Scene(SceneID id, const String& name, const String& sceneFile, const SceneSettings& settings);
	Scene(SceneID id, const String& name, const String& sceneFile, const SceneSettings& settings, Ref<Entity>& entity);
	~Scene() = default;

	Scene* findScene(SceneID scene);
	void destroy();
	void reload();

	void onLoad();
	bool snatchChild(Ref<Scene>& child);
	bool addChild(Ref<Scene>& child);
	bool removeChild(Scene* toRemove);

	void setEntity(Ref<Entity> entity) { m_Entity = entity; }
	void setName(const String& name);
	
	JSON::json getJSON() const;
	const Vector<Ref<Scene>>& getChildren() const { return m_ChildrenScenes; }
	SceneID getID() const { return m_ID; }
	Scene* getParent() const { return m_ParentScene; }
	Entity* getEntity() const { return m_Entity.get(); }
	const String& getSceneFilePath() const { return m_SceneFile; }
	const String& getName() const { return m_Name; }
	const String& getFullName() const { return m_FullName; }
	SceneSettings& getSettings() { return m_Settings; }
};
