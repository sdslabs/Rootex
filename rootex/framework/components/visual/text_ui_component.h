#pragma once

#include "components/visual/render_ui_component.h"
#include "resource_loader.h"
#include "core/resource_files/font_resource_file.h"

/// Component to render 2D UI Text
class TextUIComponent : public RenderUIComponent
{
public:
	/// DirectXTK flipping modes for sprites
	enum class Mode
	{
		None = DirectX::SpriteEffects_None,
		FlipX = DirectX::SpriteEffects_FlipHorizontally,
		FlipY = DirectX::SpriteEffects_FlipVertically,
		FlipXY = DirectX::SpriteEffects_FlipBoth,
	};

	static Component* Create(const JSON::json& componentData);

	/// Font file
	Ref<FontResourceFile> m_FontFile;
	/// Text to display
	String m_Text;
	/// Color of text
	Color m_Color;
	/// Flipping mode
	Mode m_Mode;
	/// 2D origin of the Font
	Vector2 m_Origin;

	TextUIComponent(Ref<FontResourceFile> font, const String& text, const Color& color, const Mode& mode, const Vector2& origin, const bool& isVisible);
	TextUIComponent(TextUIComponent&) = delete;
	virtual ~TextUIComponent() = default;

	friend class ECSFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::TextUIComponent;

	virtual void render() override;

	virtual ComponentID getComponentID() const override { return s_ID; }
	virtual const char* getName() const override { return "TextUIComponent"; };
	virtual JSON::json getJSON() const override;

	void setFont(Ref<FontResourceFile> fontFile) { m_FontFile = fontFile; }
	void setText(const String& text) { m_Text = text; }

	virtual void draw() override;
};
