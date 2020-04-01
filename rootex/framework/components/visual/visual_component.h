#pragma once

#include "component.h"
#include "core/renderer/renderer.h"

#include "renderer/index_buffer.h"
#include "renderer/material.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "renderer/vertex_buffer.h"

#include "components/transform_component.h"
#include "components/hierarchy_component.h"

class HierarchyGraph;

enum RenderPass
{
	RenderPassMain = 1 << 0,
	RenderPassSky = 1 << 1,
	RenderPassEditor = 1 << 2,
	RenderPassUI = 1 << 3,
	RenderPassEnd
};

class VisualComponent : public Component
{
	bool m_IsVisible;
	unsigned int m_RenderPass;

#ifdef ROOTEX_EDITOR
	String m_RenderPassName;
#endif // ROOTEX_EDITOR

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::VisualComponent;
	
	VisualComponent(const unsigned int& renderPassSetting, Ref<Material> material, VisualModelResourceFile* resFile, bool visibility);
	VisualComponent(VisualComponent&) = delete;
	virtual ~VisualComponent();

	virtual bool preRender();
	virtual bool isVisible() const;
	virtual void render();
	virtual void renderChildren(const unsigned int& renderPass);
	virtual void postRender();

	void setVisibility(bool enabled) { m_IsVisible = enabled; }
	
	const unsigned int& getRenderPass() const { return m_RenderPass; }
	virtual String getName() const override { return "VisualComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
