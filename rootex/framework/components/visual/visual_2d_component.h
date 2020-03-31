#pragma once

#include "components/visual/visual_component.h"
#include "components/transform_component.h"

class Visual2DComponent : public VisualComponent
{
protected:
	Visual2DComponent();
	Visual2DComponent(Visual2DComponent&) = default;
	virtual ~Visual2DComponent() = default;
	
	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::Visual2DComponent;

	virtual bool preRender() override;
	virtual void render() override {}
	virtual void postRender() override;

	virtual ComponentID getComponentID() const override { return s_ID; }
	virtual String getName() const override { return "Visual2DComponent"; };
};
