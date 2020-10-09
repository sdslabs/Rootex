#pragma once

#include "common/common.h"
#include "script/interpreter.h"
#include "event.h"

class Component;

typedef unsigned int ComponentID;
typedef int EntityID;

/// A collection of ECS style components that define an ECS style entity.
/// Use EntityFactory to create Entity objects.
class Entity
{
protected:
	EntityID m_ID;
	String m_Name;
	HashMap<ComponentID, Ref<Component>> m_Components;
	bool m_IsEditorOnly;
	
	String m_FullName;

	Entity(EntityID id, const String& name, const HashMap<ComponentID, Ref<Component>>& components = {});

	bool setupEntities();

	void addComponent(const Ref<Component>& component);
	friend class EntityFactory;
#ifdef ROOTEX_EDITOR
	friend class InspectorDock;
	friend class HierarchyDock;
#endif // ROOTEX_EDITOR

public:
	static void RegisterAPI(sol::table& rootex);

	virtual ~Entity();

	bool setupComponents();
	void removeComponent(Ref<Component> component);
	/// Destruct all components.
	void destroy();

	bool hasComponent(ComponentID componentID);
	
	EntityID getID() const;
	const String& getName() const;
	/// Full name consists of entity name followed by the corresponding EntityID.
	const String& getFullName() const;
	
	template <class ComponentType = Component>
	Ref<ComponentType> getComponent() const;

	template <class ComponentType = Component>
	Ref<ComponentType> getComponentFromID(ComponentID ID) const;

	JSON::json getJSON() const;
	const HashMap<ComponentID, Ref<Component>>& getAllComponents() const;
	bool isEditorOnly() const { return m_IsEditorOnly; }
	
	void setName(const String& name);
	void setEditorOnly(bool editorOnly) { m_IsEditorOnly = editorOnly; }
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
