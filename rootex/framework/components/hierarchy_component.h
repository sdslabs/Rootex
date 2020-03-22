#pragma once

#include "component.h"

#include "entity.h"

class HierarchyComponent : public Component
{
protected:
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	EntityID m_ParentID;
	Vector<EntityID> m_ChildrenIDs;

	Vector<Ref<Entity>> m_Children;
	Ref<Entity> m_Parent;

	friend class EntityFactory;
	friend class HierarchySystem;
	friend class RootHierarchyComponent;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::HierarchyComponent;

	HierarchyComponent(EntityID parentID, const Vector<EntityID>& childrenIDs);
	HierarchyComponent(HierarchyComponent&) = delete;
	virtual ~HierarchyComponent() = default;
	
	virtual bool addChild(Ref<Entity> child);
	virtual bool removeChild(Ref<Entity> node);

	virtual String getName() const override { return "HierarchyComponent"; }
	ComponentID getComponentID() const { return s_ID; }
	const Vector<Ref<Entity>>& getChildren() const { return m_Children; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
