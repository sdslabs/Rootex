#pragma once

#include "component.h"

#include "entity.h"

class HierarchyComponent : public Component
{
	static Component* Create(const LuaVariable& componentData);

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::HierarchyComponent;

	ComponentID getComponentID() const { return s_ID; }

	Vector<Ref<Entity>> m_Children;
	Ref<Entity> m_Parent = nullptr;

	bool addChild(Entity* child);
	bool removeChild(Ref<Entity> node);
};
