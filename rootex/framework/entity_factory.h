#pragma once

#include "common/common.h"
#include "resource_file.h"
#include "entity.h"
#include "component.h"

#define INVALID_ID 0
#define ROOT_ENTITY_ID 1

typedef Component* (*ComponentCreator)(const JSON::json& componentDescription);
typedef Component* (*ComponentDefaultCreator)();
typedef unsigned int EntityID;
typedef Vector<Tuple<ComponentID, String, ComponentCreator>> ComponentDatabase;
typedef Vector<Tuple<ComponentID, String, ComponentDefaultCreator>> DefaultComponentDatabase;

class EntityFactory
{
	static EntityID s_CurrentID;

	HashMap<EntityID, Ref<Entity>> m_Entities;

	EntityID getNextID();

protected:
	ComponentDatabase m_ComponentCreators;
	DefaultComponentDatabase m_DefaultComponentCreators;

	EntityFactory();
	EntityFactory(EntityFactory&) = delete;
	~EntityFactory();

	Ref<Entity> createRootEntity();
	friend class HierarchyGraph;

public:
	static void BindFunctions();
	static EntityFactory* GetSingleton();

	Ref<Component> createComponent(const String& name, const JSON::json& componentData);
	Ref<Component> createDefaultComponent(const String& name);
	Ref<Entity> createEntity(TextResourceFile* entityJSONDescription);
	Ref<Entity> findEntity(EntityID entityID);

	void addDefaultComponent(Ref<Entity> entity, String componentName);
	void addComponent(Ref<Entity> entity, Ref<Component> component);
	void destroyEntities(bool saveRoot);

	const ComponentDatabase& getComponentDatabase() const { return m_ComponentCreators; }
	const HashMap<EntityID, Ref<Entity>>& getEntities() const { return m_Entities; }
	HashMap<EntityID, Ref<Entity>>& getMutableEntities() { return m_Entities; }
};
