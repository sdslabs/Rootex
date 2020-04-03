#pragma once

#include "visual_component.h"
#include "renderer/material.h"

class DiffuseVisualComponent : public VisualComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	ImageResourceFile* m_ImageFile;
	
	DiffuseVisualComponent(const unsigned int& renderPass, Ref<DiffuseMaterial> material, VisualModelResourceFile* resFile, ImageResourceFile* imageRes, Texture* texture, bool visibility);
	virtual ~DiffuseVisualComponent();

	friend class EntityFactory;

#ifdef ROOTEX_EDITOR
	String m_ImagePathUI;
#endif // ROOTEX_EDITOR

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::DiffuseVisualComponent;

	bool preRender() override;

	virtual String getName() const override { return "DiffuseVisualComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
	void setTexture(ImageResourceFile* image);
#endif // ROOTEX_EDITOR
};
