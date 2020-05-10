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

	Vector<HierarchyComponent*> m_Children;
	HierarchyComponent* m_Parent;

	friend class EntityFactory;
	friend class HierarchySystem;
	
public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::HierarchyComponent;

	HierarchyComponent(EntityID parentID, const Vector<EntityID>& childrenIDs);
	HierarchyComponent(HierarchyComponent&) = delete;
	virtual ~HierarchyComponent() = default;
	
	virtual bool addChild(Ref<Entity> child);
	virtual bool removeChild(Ref<Entity> node);
	virtual bool snatchChild(Ref<Entity> node);
	void clear();

	virtual void onRemove() override;
	virtual String getName() const override { return "HierarchyComponent"; }
	ComponentID getComponentID() const { return s_ID; }
	HierarchyComponent* getParent() const { return m_Parent; }
	const Vector<HierarchyComponent*>& getChildren() const { return m_Children; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
