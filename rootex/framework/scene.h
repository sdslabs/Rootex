#pragma once

#include "common/common.h"
#include "entity.h"

typedef unsigned int SceneID;

#define ROOT_SCENE_ID 0

class Scene
{
	SceneID m_ID;
	String m_Name;
	Ref<Entity> m_Entity;
	
	Scene* m_ParentScene = nullptr;
	Vector<Ref<Scene>> m_ChildrenScenes;

public:
	static Ref<Scene> Create(const JSON::json& sceneData);
	static Ref<Scene> CreateRootScene();

	Scene(SceneID id, const String& name);
	Scene(SceneID id, const String& name, Ref<Entity>& entity);

	Scene* findScene(Scene* scene);
	
	bool snatchChild(Ref<Scene>& child);
	bool addChild(Ref<Scene>& child);
	bool removeChild(Scene* toRemove);

	void setEntity(Ref<Entity> entity) { m_Entity = entity; }
	void setName(const String& name) { m_Name = name; }
	
	JSON::json getJSON() const;
	const Vector<Ref<Scene>>& getChildren() const { return m_ChildrenScenes; }
	SceneID getID() const { return m_ID; }
	Scene* getParent() const { return m_ParentScene; }
	const String& getName() const { return m_Name; }
};
