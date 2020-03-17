#pragma once

#include "common/common.h"
#include "visual/visual_component.h"
#include "components/hierarchy_component.h"

class RootHierarchyComponent : public HierarchyComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	Ref<HierarchyComponent> m_StaticGroup;
	Ref<HierarchyComponent> m_EntityGroup;
	Ref<HierarchyComponent> m_GlobalGroup;
	Ref<HierarchyComponent> m_SkyGroup;
	Ref<HierarchyComponent> m_EditorGroup;
	Ref<HierarchyComponent> m_UIGroup;

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::RootHierarchyComponent;

	RootHierarchyComponent(EntityID parentID, const Vector<EntityID>& childrenIDs);
	RootHierarchyComponent(RootHierarchyComponent&) = delete;
	virtual ~RootHierarchyComponent();

	bool isVisible(HierarchyGraph* graph);
	bool preRender(HierarchyGraph* graph);
	void renderPassRender(Ref<HierarchyComponent> renderPassGraph, HierarchyGraph* graph);
	void renderChildren(HierarchyGraph* graph);
	void postRender(HierarchyGraph* graph);
	
	virtual bool addChild(Ref<Entity> child) override;
	void clear();

	virtual String getName() const override { return "RootHierarchyComponent"; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
