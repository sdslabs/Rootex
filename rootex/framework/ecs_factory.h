#pragma once

#include "common/common.h"
#include "core/resource_files/text_resource_file.h"

#include "entity.h"
#include "component.h"

/// Function pointer to a function that constructs a component, taking in a set of component data.
typedef Component* (*ComponentCreator)(const JSON::json& componentDescription);
/// Collection of a component, its name, and a function that constructs that component.
typedef Vector<Tuple<ComponentID, String, ComponentCreator>> ComponentDatabase;

#define REGISTER_COMPONENT(ComponentType) ECSFactory::RegisterComponent<ComponentType>(#ComponentType)

class ECSFactory
{
	static inline ComponentDatabase s_ComponentCreators;

public:
	static bool AddComponent(Ref<Entity>& entity, Ref<Component> component);
	static Ref<Component> CreateComponent(const String& componentName, const JSON::json& componentData);
	static Ref<Component> CreateDefaultComponent(const String& componentName);

	static Ref<Entity> CreateEntity(Scene* scene, const JSON::json& entityJSON);
	static Ref<Entity> CreateEntity(Scene* scene, TextResourceFile* textResourceFile);
	static Ref<Entity> CreateEmptyEntity(Scene* scene);
	static Ref<Entity> CreateRootEntity(Scene* scene);
	static Ref<Entity> CopyEntity(Scene* scene, Entity& entity);
	
	template <class T>
	static void RegisterComponent(const String& name);

	static void Initialize();
};

template <class T>
inline void ECSFactory::RegisterComponent(const String& name)
{
	s_ComponentCreators.push_back({ T::s_ID, name, T::Create });
}
