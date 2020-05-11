#pragma once

#include "component.h"
#include "components/transform_component.h"

/// Our UI base class
class UIComponent : public Component
{
protected:
	UIComponent();
	UIComponent(UIComponent&) = default;
	virtual ~UIComponent() = default;
	
	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::UIComponent;

	virtual bool preRender() override;
	virtual void render(RenderPass renderPass) override {}
	virtual void postRender() override;

	virtual ComponentID getComponentID() const override { return s_ID; }
	virtual String getName() const override { return "UIComponent"; };
};
