#pragma once

#include "model_visual_component.h"
#include "renderer/material.h"

class TexturedModelVisualComponent : public ModelVisualComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	ImageResourceFile* m_ImageFile;
	
	TexturedModelVisualComponent(const unsigned int& renderPass, Ref<TexturedMaterial> material, VisualModelResourceFile* resFile, ImageResourceFile* imageRes, Texture* texture, bool visibility);
	virtual ~TexturedModelVisualComponent();

	friend class EntityFactory;

#ifdef ROOTEX_EDITOR
	String m_ImagePathUI;
#endif // ROOTEX_EDITOR

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::TexturedVisualComponent;

	void render(RenderPass renderPass) override;

	void setTexture(ImageResourceFile* image);

	virtual String getName() const override { return "TexturedModelVisualComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
