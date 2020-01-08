#pragma once

#include "visual_component.h"
#include "renderer/texture.h"

class DiffuseVisualComponent : public VisualComponent
{
	static Component* Create(const LuaVariable& componentData);

	DiffuseVisualComponent();
	virtual ~DiffuseVisualComponent();

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::DiffuseVisualComponent;

	ComponentID getComponentID() const override { return s_ID; }
};
