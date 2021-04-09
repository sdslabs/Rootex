#pragma once

#include "resource_loader.h"
#include "components/visual/ui/render_ui_component.h"
#include "core/resource_files/font_resource_file.h"

/// Component to render 2D UI Text
class TextUIComponent : public RenderUIComponent
{
	DEFINE_COMPONENT(TextUIComponent, Category::UI);

public:
	/// DirectXTK flipping modes for sprites
	enum class Mode
	{
		None = DirectX::SpriteEffects_None,
		FlipX = DirectX::SpriteEffects_FlipHorizontally,
		FlipY = DirectX::SpriteEffects_FlipVertically,
		FlipXY = DirectX::SpriteEffects_FlipBoth,
	};

private:
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
	/// Flat rotation
	float m_Rotation;

public:
	TextUIComponent(Entity& owner, const JSON::json& data);
	~TextUIComponent() = default;

	void setFont(Ref<FontResourceFile> fontFile) { m_FontFile = fontFile; }
	void setText(const String& text) { m_Text = text; }

	void render() override;

	JSON::json getJSON() const override;
	void draw() override;
};
