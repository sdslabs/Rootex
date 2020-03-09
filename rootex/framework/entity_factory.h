#pragma once

#include "common/common.h"
#include "resource_file.h"
#include "script/interpreter.h"
#include "entity.h"

#define INVALID_ID 0

class Component;
class Entity;

typedef Component* (*ComponentCreator)(const LuaVariable& componentDescription);
typedef unsigned int EntityID;

class EntityFactory
{
	static EntityID s_CurrentID;

	Vector<Ref<Entity>> m_Entities;

	EntityID getNextID();

protected:
	HashMap<String, ComponentCreator> m_ComponentCreators;

	Ref<Component> createComponent(const String& name, const LuaVariable& componentData);
	Ref<Component> EntityFactory::createHierarchyComponent();

	EntityFactory();
	EntityFactory(EntityFactory&) = delete;
	~EntityFactory();

	Ref<Entity> createRootEntity();
	friend class HierarchyGraph;

public:
	static EntityFactory* GetSingleton();

	Ref<Entity> createEntity(LuaTextResourceFile* actorLuaDescription);

	void destroyEntities();
};
