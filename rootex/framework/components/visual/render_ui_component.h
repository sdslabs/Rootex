#pragma once

#include "component.h"
#include "components/transform_component.h"

/// Our UI base class
class RenderUIComponent : public Component
{
	DEPENDS_ON(TransformComponent);

protected:
	bool m_IsVisible;

	RenderUIComponent(bool isVisible);
	RenderUIComponent(RenderUIComponent&) = delete;
	virtual ~RenderUIComponent() = default;
	
	friend class ECSFactory;

public:
	static void RegisterAPI(sol::table& rootex);
	
	bool preRender();
	virtual void render() = 0;
	void postRender();

	void setIsVisible(bool enabled) { m_IsVisible = enabled; }
	bool isVisible() const { return m_IsVisible; }

	virtual JSON::json getJSON() const override;
};
