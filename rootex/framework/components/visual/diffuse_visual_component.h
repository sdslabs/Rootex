#pragma once

#include "visual_component.h"
#include "renderer/material.h"

class DiffuseVisualComponent : public VisualComponent
{
	static Component* Create(const LuaVariable& componentData);

	Ref<DiffuseMaterial> m_DiffuseMaterial;

	DiffuseVisualComponent(RenderPass renderPass, Ref<DiffuseMaterial> material, VisualModelResourceFile* resFile);
	virtual ~DiffuseVisualComponent();

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::VisualComponent;

	bool preRender(HierarchyGraph* graph) override;

	virtual String getName() const override { return "DiffuseVisualComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
};
