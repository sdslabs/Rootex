#pragma once

#include "component.h"

#undef interface
#include "RmlUi/Core.h"
#define interface __STRUCT__

class UIComponent : public Component
{
	static Component* Create(const JSON::json& componentData);

	friend class EntityFactory;

	String m_FilePath;
	Rml::ElementDocument* m_Document;

	UIComponent(const String& path);
	UIComponent(const UIComponent&) = delete;
	virtual ~UIComponent();

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::UIComponent;

	Rml::ElementDocument* getDocument() { return m_Document; };
	void setDocument(const String& path);

	virtual const char* getName() const override { return "UIComponent"; }
	ComponentID getComponentID() const { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	virtual void draw() override;
#endif // ROOTEX_EDITOR
};
