#pragma once

#include "components/visual/visual_2d_component.h"
#include "resource_loader.h"

class TextVisual2DComponent : public Visual2DComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	FontResourceFile* m_Font;
	String m_Text;

	TextVisual2DComponent(FontResourceFile* font, const String& text);
	virtual ~TextVisual2DComponent();

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::Visual2DComponent;

	virtual void render(HierarchyGraph* graph) override;

	virtual ComponentID getComponentID() const override { return s_ID; }
	virtual String getName() const override { return "TextVisual2DComponent"; };
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	virtual void draw() override;
#endif
};
