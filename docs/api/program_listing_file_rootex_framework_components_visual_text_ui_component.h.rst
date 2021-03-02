
.. _program_listing_file_rootex_framework_components_visual_text_ui_component.h:

Program Listing for File text_ui_component.h
============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_text_ui_component.h>` (``rootex\framework\components\visual\text_ui_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "components/visual/render_ui_component.h"
   #include "resource_loader.h"
   #include "core/resource_files/font_resource_file.h"
   
   class TextUIComponent : public RenderUIComponent
   {
   public:
       enum class Mode
       {
           None = DirectX::SpriteEffects_None,
           FlipX = DirectX::SpriteEffects_FlipHorizontally,
           FlipY = DirectX::SpriteEffects_FlipVertically,
           FlipXY = DirectX::SpriteEffects_FlipBoth,
       };
   
       static Component* Create(const JSON::json& componentData);
   
       FontResourceFile* m_FontFile;
       String m_Text;
       Color m_Color;
       Mode m_Mode;
       Vector2 m_Origin;
   
       TextUIComponent(FontResourceFile* font, const String& text, const Color& color, const Mode& mode, const Vector2& origin, const bool& isVisible);
       TextUIComponent(TextUIComponent&) = delete;
       virtual ~TextUIComponent() = default;
   
       friend class ECSFactory;
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::TextUIComponent;
   
       virtual void render() override;
   
       virtual ComponentID getComponentID() const override { return s_ID; }
       virtual const char* getName() const override { return "TextUIComponent"; };
       virtual JSON::json getJSON() const override;
   
       void setFont(FontResourceFile* fontFile) { m_FontFile = fontFile; }
       void setText(const String& text) { m_Text = text; }
   
       virtual void draw() override;
   };
