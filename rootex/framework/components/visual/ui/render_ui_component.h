#pragma once

#include "component.h"
#include "components/space/transform_component.h"

/// Our UI base class used to render UI with RenderingDevice
class RenderUIComponent : public Component
{
	DEPENDS_ON(TransformComponent);

protected:
	bool m_IsVisible;

	RenderUIComponent(bool isVisible);

public:
	virtual ~RenderUIComponent() = default;

	bool preRender();
	virtual void render() = 0;
	void postRender();

	void setIsVisible(bool enabled) { m_IsVisible = enabled; }
	bool isVisible() const { return m_IsVisible; }

	JSON::json getJSON() const override;
};
