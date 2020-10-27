#pragma once

#include "common/common.h"
#include "script/interpreter.h"
#include "event.h"

class Component;
class Scene;

typedef unsigned int ComponentID;
typedef int EntityID;

/// A collection of ECS style components that define an ECS style entity.
class Entity
{
protected:
	Scene* m_Scene;
	HashMap<ComponentID, Ref<Component>> m_Components;
	
	friend class ECSFactory;

public:
	static void RegisterAPI(sol::table& rootex);

	Entity(Scene* scene);
	Entity(const Entity&) = delete;
	~Entity();

	bool onAllComponentsAdded();
	bool onAllEntitiesAdded();
	bool removeComponent(ComponentID toRemoveComponentID, bool hardRemove = false);
	bool hasComponent(ComponentID componentID);

	/// Destruct all components. Use force for silent deletion of components
	void destroy(bool force = false);
	
	Scene* getScene() const { return m_Scene; }
	
	template <class ComponentType = Component>
	Ref<ComponentType> getComponent() const;

	template <class ComponentType = Component>
	Ref<ComponentType> getComponentFromID(ComponentID ID) const;

	JSON::json getJSON() const;
	const String& getName() const;
	const String& getFullName() const;
	const HashMap<ComponentID, Ref<Component>>& getAllComponents() const;
};

template <class ComponentType>
inline Ref<ComponentType> Entity::getComponent() const
{
	auto findIt = m_Components.find(ComponentType::s_ID);
	if (findIt != m_Components.end())
	{
		Ref<Component> baseTypeComponent = findIt->second;
		return std::dynamic_pointer_cast<ComponentType>(baseTypeComponent);
	}

	return nullptr;
}

template <class ComponentType>
inline Ref<ComponentType> Entity::getComponentFromID(ComponentID ID) const
{
	auto findIt = m_Components.find(ID);
	if (findIt != m_Components.end())
	{
		Ref<Component> baseTypeComponent = findIt->second;
		return std::dynamic_pointer_cast<ComponentType>(baseTypeComponent);
	}

	return nullptr;
}
