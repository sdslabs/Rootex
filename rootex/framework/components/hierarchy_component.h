#pragma once

#include "component.h"

#include "entity.h"

class HierarchyComponent : public Component
{
protected:
	static Component* Create(const LuaVariable& componentData);

	Vector<Ref<Entity>> m_Children;
	Ref<Entity> m_Parent = nullptr;

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::HierarchyComponent;

	virtual String getName() const override { return "HierarchyComponent"; }
	ComponentID getComponentID() const { return s_ID; }

	virtual bool addChild(Ref<Entity> child);
	virtual bool removeChild(Ref<Entity> node);

	const Vector<Ref<Entity>>& getChildren() const { return m_Children; }

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
