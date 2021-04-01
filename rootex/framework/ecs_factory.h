#pragma once

#include "common/common.h"
#include "core/resource_files/text_resource_file.h"

#include "entity.h"
#include "component.h"

/// Function pointer to a function that constructs a component, taking in a set of component data.
typedef Ptr<Component> (*ComponentCreator)(const JSON::json& componentDescription);

struct ComponentTypeData
{
	String componentName;
	ComponentID componentID;
	String category;
	ComponentCreator creator;
};

/// Collection of meta info about a component.
typedef Vector<ComponentTypeData> ComponentDatabase;
/// Collection of all components active inside the scene.
typedef HashMap<ComponentID, Vector<Component*>> ComponentInstanceDatabase;

#define REGISTER_COMPONENT(ComponentType, componentCategory) ECSFactory::RegisterComponent<ComponentType>(#ComponentType, componentCategory);

class ECSFactory
{
	struct Category
	{
		static inline const String General = "General";
		static inline const String Audio = "Audio";
		static inline const String Game = "Game";
		static inline const String Physics = "Physics";
		static inline const String Model = "Model";
		static inline const String Effect = "Effect";
		static inline const String Light = "Light";
		static inline const String UI = "UI";
	};

	static inline ComponentDatabase s_ComponentCreators;
	static inline ComponentInstanceDatabase s_ComponentInstances;

public:
	template <class T>
	static Vector<Component*>& GetComponents();

	static void Initialize();

	static bool AddComponent(Entity* entity, Ptr<Component>& component);
	static Ptr<Component> CreateComponent(const String& componentName, const JSON::json& componentData);
	static Ptr<Component> CreateDefaultComponent(const String& componentName);

	static Ptr<Entity> CreateEntity(Scene* scene, const JSON::json& entityJSON);
	static Ptr<Entity> CreateEntityFromFile(Scene* scene, TextResourceFile* textResourceFile);
	static Ptr<Entity> CreateEmptyEntity(Scene* scene);
	static Ptr<Entity> CreateRootEntity(Scene* scene);
	static Ptr<Entity> CopyEntity(Scene* scene, Entity& entity);

	static String GetComponentNameByID(ComponentID componentID);
	static const ComponentDatabase& GetComponentDatabase() { return s_ComponentCreators; }

	template <class T>
	static void RegisterComponent(const String& name, const String& category);

	static void RegisterComponentInstance(Component* component);
	static void DeregisterComponentInstance(Component* component);
};

template <class T>
inline Vector<Component*>& ECSFactory::GetComponents()
{
	return s_ComponentInstances[T::s_ID];
}

template <class T>
inline void ECSFactory::RegisterComponent(const String& name, const String& category)
{
	ComponentTypeData data;
	data.category = category;
	data.componentID = T::s_ID;
	data.componentName = name;
	data.creator = T::Create;

	s_ComponentCreators.push_back(data);
}
