#pragma once

#include "components/visual/visual_2d_component.h"
#include "resource_loader.h"

/// Component to render 2D UI Text
class TextVisual2DComponent : public Visual2DComponent
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
	static Component* CreateDefault();

	/// Font file
	FontResourceFile* m_FontFile;
	/// Text to display
	String m_Text;
	/// Color of text
	Color m_Color;
	/// Flipping mode
	Mode m_Mode;
	/// 2D origin of the Font
	Vector2 m_Origin;

	TextVisual2DComponent(FontResourceFile* font, const String& text, const Color& color, const Mode& mode, const Vector2& origin);
	virtual ~TextVisual2DComponent();

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::Visual2DComponent;

	virtual void render() override;

	virtual ComponentID getComponentID() const override { return s_ID; }
	virtual String getName() const override { return "TextVisual2DComponent"; };
	virtual JSON::json getJSON() const override;

	void setFont(FontResourceFile* fontFile) { m_FontFile = fontFile; }
	void setText(const String& text) { m_Text = text; }
#ifdef ROOTEX_EDITOR
	virtual void draw() override;
#endif
};
