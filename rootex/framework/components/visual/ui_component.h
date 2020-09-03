#pragma once

#include "component.h"

#undef interface
#include "RmlUi/Core.h"
#define interface __STRUCT__

class UIComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	friend class EntityFactory;

	String m_FilePath;
	Rml::Core::ElementDocument* m_Document;

	UIComponent(const String& path);
	UIComponent(const UIComponent&) = delete;
	virtual ~UIComponent() = default;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::UIComponent;

	void setDocument(const String& path);
	void onRemove() override;

	virtual String getName() const override { return "UIComponent"; }
	ComponentID getComponentID() const { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	virtual void draw() override;
#endif // ROOTEX_EDITOR
};
