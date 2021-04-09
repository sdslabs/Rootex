#pragma once

#include "component.h"

#undef interface
#include "RmlUi/Core.h"
#define interface __STRUCT__

class UIComponent : public Component
{
	DEFINE_COMPONENT(UIComponent, Category::UI);

	String m_FilePath;
	Rml::ElementDocument* m_Document;

public:
	UIComponent(Entity& owner, const JSON::json& data);
	~UIComponent();

	Rml::ElementDocument* getDocument() { return m_Document; };
	void setDocument(const String& path);

	JSON::json getJSON() const override;
	void draw() override;
};
