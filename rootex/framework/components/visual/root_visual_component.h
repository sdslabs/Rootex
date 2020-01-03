#pragma once

#include "common/common.h"
#include "visual_component.h"

class RootVisualComponent : public VisualComponent
{
	Ref<VisualComponent> m_StaticGroup;
	Ref<VisualComponent> m_EntityGroup;
	Ref<VisualComponent> m_GlobalGroup;
	Ref<VisualComponent> m_SkyGroup;
	Ref<VisualComponent> m_EditorGroup;

public:
	RootVisualComponent();
	~RootVisualComponent();

	virtual bool preRender(VisualComponentGraph* visualComponentGraph) override;
	virtual bool addChild(VisualComponent* child) override;
	virtual void renderChildren(VisualComponentGraph* visualComponentGraph) override;
	virtual bool isVisible(VisualComponentGraph* visualComponentGraph) const;
	virtual void postRender(VisualComponentGraph* visualComponentGraph) override;
};
