#pragma once

#include "model_visual_component.h"
#include "renderer/materials/textured_material.h"

class TexturedModelVisualComponent : public ModelVisualComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();
	
	TexturedModelVisualComponent(const unsigned int& renderPass, Ref<TexturedMaterial> material, VisualModelResourceFile* resFile, bool visibility);
	virtual ~TexturedModelVisualComponent();

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::TexturedVisualComponent;

	void render(RenderPass renderPass) override;

	virtual String getName() const override { return "TexturedModelVisualComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
