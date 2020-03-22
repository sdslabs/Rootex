#pragma once

#include "common/common.h"
#include "script/interpreter.h"
#include "event.h"

class Component;
typedef unsigned int ComponentID;
typedef unsigned int EntityID;

class Entity
{
protected:
	EntityID m_ID;
	String m_Name;
	HashMap<ComponentID, Ref<Component>> m_Components;
	
	Entity(EntityID id, const String& name, const HashMap<ComponentID, Ref<Component>>& components = {});

	bool setupComponents();

	void addComponent(const Ref<Component>& component);
	friend class EntityFactory;
#ifdef ROOTEX_EDITOR
	friend class InspectorDock;
#endif // ROOTEX_EDITOR

public:
	virtual ~Entity();

	void addChild(Ref<Entity> child);
	void removeComponent(Ref<Component> component);
	void destroy();

	bool hasComponent(ComponentID componentID);
	
	EntityID getID() const;
	const String& getName() const;
	
	template <class ComponentType = Component>
	Ref<ComponentType> getComponent(ComponentID ID = ComponentType::s_ID) const;

	Component* getComponentPointer(ComponentID ID);
	JSON::json getJSON() const;
	const HashMap<ComponentID, Ref<Component>>& getAllComponents() const;
	
	void setName(const String& name);
};

template <class ComponentType>
inline Ref<ComponentType> Entity::getComponent(ComponentID ID) const
{
	auto findIt = m_Components.find(ID);
	if (findIt != m_Components.end())
	{
		Ref<Component> baseTypeComponent = findIt->second;
		return std::dynamic_pointer_cast<ComponentType>(baseTypeComponent);
	}

	return nullptr;
}
