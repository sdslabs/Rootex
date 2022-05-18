
.. _program_listing_file_rootex_framework_components_visual_ui_ui_component.h:

Program Listing for File ui_component.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_visual_ui_ui_component.h>` (``rootex\framework\components\visual\ui\ui_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "component.h"
   
   #undef interface
   #include "RmlUi/Core.h"
   #define interface __STRUCT__
   
   class UIComponent : public Component
   {
       COMPONENT(UIComponent, Category::UI);
   
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
   
   DECLARE_COMPONENT(UIComponent);
