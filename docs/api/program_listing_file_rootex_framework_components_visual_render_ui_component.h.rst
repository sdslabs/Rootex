
.. _program_listing_file_rootex_framework_components_visual_render_ui_component.h:

Program Listing for File render_ui_component.h
==============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_render_ui_component.h>` (``rootex\framework\components\visual\render_ui_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "component.h"
   #include "components/transform_component.h"
   
   class RenderUIComponent : public Component
   {
       DEPENDS_ON(TransformComponent);
   
   protected:
       bool m_IsVisible;
   
       RenderUIComponent(bool isVisible);
       RenderUIComponent(RenderUIComponent&) = delete;
       virtual ~RenderUIComponent() = default;
   
       friend class ECSFactory;
   
   public:
       bool preRender();
       virtual void render() = 0;
       void postRender();
   
       void setIsVisible(bool enabled) { m_IsVisible = enabled; }
       bool isVisible() const { return m_IsVisible; }
   
       virtual JSON::json getJSON() const override;
   };
