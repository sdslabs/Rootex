#pragma once

#include "component.h"
#include "components/transform_component.h"

/// Our UI base class
class UIComponent : public Component
{
protected:
	bool m_IsVisible;

	TransformComponent* m_TransformComponent;

	UIComponent(bool isVisible);
	UIComponent(UIComponent&) = delete;
	virtual ~UIComponent() = default;
	
	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::UIComponent;

	virtual bool setup() override;

	bool preRender();
	virtual void render() = 0;
	void postRender();

	void setIsVisible(bool enabled) { m_IsVisible = enabled; }
	bool isVisible() const { return m_IsVisible; }

	virtual ComponentID getComponentID() const override { return s_ID; }
	virtual String getName() const override { return "UIComponent"; };
	virtual JSON::json getJSON() const override;
};
