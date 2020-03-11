#pragma once

#include "common/common.h"
#include "resource_file.h"
#include "script/interpreter.h"
#include "entity.h"
#include "component.h"

#define INVALID_ID 0

typedef Component* (*ComponentCreator)(const LuaVariable& componentDescription);
typedef Component* (*ComponentDefaultCreator)();
typedef unsigned int EntityID;

class EntityFactory
{
	static EntityID s_CurrentID;

	Vector<Ref<Entity>> m_Entities;

	EntityID getNextID();

protected:
	typedef Vector<Tuple<ComponentID, String, ComponentCreator>> ComponentDatabase;
	ComponentDatabase m_ComponentCreators;
	typedef Vector<Tuple<ComponentID, String, ComponentDefaultCreator>> DefaultComponentDatabase;
	DefaultComponentDatabase m_DefaultComponentCreators;

	Ref<Component> EntityFactory::createHierarchyComponent();

	EntityFactory();
	EntityFactory(EntityFactory&) = delete;
	~EntityFactory();

	Ref<Entity> createRootEntity();
	friend class HierarchyGraph;

public:
	static EntityFactory* GetSingleton();

	Ref<Component> createComponent(const String& name, const LuaVariable& componentData);
	Ref<Component> createDefaultComponent(const String& name);
	Ref<Entity> createEntity(LuaTextResourceFile* actorLuaDescription);

	void addDefaultComponent(Ref<Entity> entity, String componentName);
	void addComponent(Ref<Entity> entity, Ref<Component> component);
	void destroyEntities();

	const ComponentDatabase& getComponentDatabase() const { return m_ComponentCreators; }
	const Vector<Ref<Entity>>& getEntities() const { return m_Entities; }
};
