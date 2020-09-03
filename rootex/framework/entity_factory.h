#pragma once

#include "common/common.h"
#include "component.h"
#include "entity.h"
#include "resource_file.h"

/// Invalid ID for an entity
#define INVALID_ID 0
/// Root entity ID
#define ROOT_ENTITY_ID 1

/// Function pointer to a function that constructs a component, taking in a set of component data.
typedef Component* (*ComponentCreator)(const JSON::json& componentDescription);
/// Function pointer to a function that default constructs a component.
typedef Component* (*ComponentDefaultCreator)();
typedef int EntityID;
/// Collection of a component, its name, and a function that constructs that component.
typedef Vector<Tuple<ComponentID, String, ComponentCreator>> ComponentDatabase;
/// Collection of a component, its name, and a function that constructs a default component.
typedef Vector<Tuple<ComponentID, String, ComponentDefaultCreator>> DefaultComponentDatabase;

class EntityFactory
{
	static EntityID s_CurrentID;
	static EntityID s_CurrentEditorID;

	HashMap<EntityID, Ref<Entity>> m_Entities;

	EntityID getNextID();
	EntityID getNextEditorID();
	String saveEntityAsClassRecursively(Ref<Entity> entity, const String& path);
	Ref<Entity> createEntityHierarchyFromClass(JSON::json entityJSON);
	void fixParentID(Ref<Entity> entity, EntityID id);

protected:
	ComponentDatabase m_ComponentCreators;
	DefaultComponentDatabase m_DefaultComponentCreators;

	EntityFactory();
	EntityFactory(EntityFactory&) = delete;
	~EntityFactory() = default;

	Ref<Entity> createRootEntity();
	friend class HierarchyGraph;

	Variant deleteEntityEvent(const Event* event);

public:
	static void RegisterAPI(sol::state& rootex);
	static EntityFactory* GetSingleton();

	Ref<Component> createComponent(const String& name, const JSON::json& componentData);
	Ref<Component> createDefaultComponent(const String& name);
	Ref<Entity> createEntity(const JSON::json& entityJSON, const String& filePath, bool isEditorOnly = false);
	Ref<Entity> createEntity(TextResourceFile* textResourceFile, bool isEditorOnly = false);
	/// Get entity by ID.
	Ref<Entity> findEntity(EntityID entityID);

	void setupLiveEntities();

	void addDefaultComponent(Ref<Entity> entity, String componentName);
	void addComponent(Ref<Entity> entity, Ref<Component> component);
	/// Pass in a boolean that determines whether the Root entity should be saved from destruction or not.
	void destroyEntities(bool saveRoot);
	void deleteEntity(Ref<Entity> entity);
	bool saveEntityAsClass(Ref<Entity> entity);
	Ref<Entity> createEntityFromClass(const JSON::json& entityJSON);
	Ref<Entity> createEntityFromClass(TextResourceFile* entityJSON);

	const ComponentDatabase& getComponentDatabase() const { return m_ComponentCreators; }
	const HashMap<EntityID, Ref<Entity>>& getEntities() const { return m_Entities; }
	HashMap<EntityID, Ref<Entity>>& getMutableEntities() { return m_Entities; }
};
