#pragma once

#include "common/common.h"
#include "visual_component.h"
#include "diffuse_visual_component.h"
#include "components/hierarchy_component.h"

class RootVisualHierarchyComponent : public VisualComponent, public HierarchyComponent
{
	HierarchyComponent m_StaticGroup;
	HierarchyComponent m_EntityGroup;
	HierarchyComponent m_GlobalGroup;
	HierarchyComponent m_SkyGroup;
	HierarchyComponent m_EditorGroup;

public:
	RootVisualHierarchyComponent();
	~RootVisualHierarchyComponent();

	virtual bool preRender(VisualComponentGraph* visualComponentGraph) override;
	virtual bool addChild(Ref<Entity> child) override;
	virtual void renderChildren(VisualComponentGraph* visualComponentGraph) override;
	virtual bool isVisible(VisualComponentGraph* visualComponentGraph) const;
	virtual void postRender(VisualComponentGraph* visualComponentGraph) override;
};
