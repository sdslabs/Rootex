
.. _program_listing_file_rootex_framework_components_visual_ui_text_ui_component.h:

Program Listing for File text_ui_component.h
============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_ui_text_ui_component.h>` (``rootex\framework\components\visual\ui\text_ui_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "resource_loader.h"
   #include "components/visual/ui/render_ui_component.h"
   #include "core/resource_files/font_resource_file.h"
   
   class TextUIComponent : public RenderUIComponent
   {
       COMPONENT(TextUIComponent, Category::UI);
   
   public:
       enum class Mode
       {
           None = DirectX::SpriteEffects_None,
           FlipX = DirectX::SpriteEffects_FlipHorizontally,
           FlipY = DirectX::SpriteEffects_FlipVertically,
           FlipXY = DirectX::SpriteEffects_FlipBoth,
       };
   
   private:
       Ref<FontResourceFile> m_FontFile;
       String m_Text;
       Color m_Color;
       Mode m_Mode;
       Vector2 m_Origin;
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
   
   DECLARE_COMPONENT(TextUIComponent);
