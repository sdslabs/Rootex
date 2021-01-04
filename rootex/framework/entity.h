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
	HashMap<ComponentID, Ptr<Component>> m_Components;
	
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

	/// Destruct all components.
	void destroy();
	
	Scene* getScene() const { return m_Scene; }
	
	template <class ComponentType = Component>
	ComponentType* getComponent();

	template <class ComponentType = Component>
	ComponentType* getComponentFromID(ComponentID ID);

	JSON::json getJSON() const;
	const String& getName() const;
	const String& getFullName() const;
	const HashMap<ComponentID, Ptr<Component>>& getAllComponents() const;
};

template <class ComponentType>
inline ComponentType* Entity::getComponent()
{
	auto findIt = m_Components.find(ComponentType::s_ID);
	if (findIt != m_Components.end())
	{
		Ptr<Component>& baseTypeComponent = findIt->second;
		return dynamic_cast<ComponentType*>(baseTypeComponent.get());
	}

	return nullptr;
}

template <class ComponentType>
inline ComponentType* Entity::getComponentFromID(ComponentID ID)
{
	auto findIt = m_Components.find(ID);
	if (findIt != m_Components.end())
	{
		Ptr<Component>& baseTypeComponent = findIt->second;
		return dynamic_cast<ComponentType*>(baseTypeComponent.get());
	}

	return nullptr;
}
