#pragma once

#include "common/common.h"
#include "visual/visual_component.h"
#include "components/hierarchy_component.h"

class RootHierarchyComponent : public HierarchyComponent
{
	HierarchyComponent m_StaticGroup;
	HierarchyComponent m_EntityGroup;
	HierarchyComponent m_GlobalGroup;
	HierarchyComponent m_SkyGroup;
	HierarchyComponent m_EditorGroup;

	friend class EntityFactory;

public:
	RootHierarchyComponent(EntityID parentID, const Vector<EntityID>& childrenIDs);
	RootHierarchyComponent(RootHierarchyComponent&) = delete;
	virtual ~RootHierarchyComponent();

	bool isVisible(HierarchyGraph* graph);
	bool preRender(HierarchyGraph* graph);
	void renderPassRender(HierarchyComponent& renderPassGraph, HierarchyGraph* graph);
	void renderChildren(HierarchyGraph* graph);
	void postRender(HierarchyGraph* graph);
	
	virtual bool addChild(Ref<Entity> child) override;

	virtual String getName() const override { return "RootHierarchyComponent"; }
	virtual JSON::json getJSON() const override;
};
