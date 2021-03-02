
.. _program_listing_file_editor_editor_system.h:

Program Listing for File editor_system.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_editor_editor_system.h>` (``editor\editor_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "core/event_manager.h"
   #include "script/interpreter.h"
   #include "framework/system.h"
   #include "utility/imgui_helpers.h"
   
   #include "gui/scene_dock.h"
   #include "gui/output_dock.h"
   #include "gui/toolbar_dock.h"
   #include "gui/viewport_dock.h"
   #include "gui/inspector_dock.h"
   #include "gui/file_viewer.h"
   
   #include "vendor/ImGUI/imgui.h"
   #include "vendor/ImGUI/imgui_impl_dx11.h"
   #include "vendor/ImGUI/imgui_impl_win32.h"
   
   #include "Tracy/Tracy.hpp"
   
   class HierarchyGraph;
   
   ImColor ColorToImColor(Color& c);
   
   class EditorSystem : public System
   {
       int m_DockSpaceID;
       String m_MenuAction;
       String m_PopupCause;
       unsigned int m_EditorStyleColorPushCount;
       unsigned int m_EditorStyleVarPushCount;
       bool m_WireframeMode = false;
       bool m_WorldMode = true;
   
       ImFont* m_EditorFont;
       ImFont* m_EditorFontItalic;
       ImFont* m_EditorFontBold;
       ImFont* m_EditorFontMonospace;
   
       struct Icons
       {
           const char* lua = ICON_ROOTEX_FILE_CODE_O;
           const char* font = ICON_ROOTEX_FONT;
           const char* text = ICON_ROOTEX_FILE_TEXT;
           const char* audio = ICON_ROOTEX_FILE_AUDIO_O;
           const char* model = ICON_ROOTEX_FORT_AWESOME;
           const char* image = ICON_ROOTEX_FILE_IMAGE_O;
       } m_Icons;
   
       Ptr<SceneDock> m_Scene;
       Ptr<OutputDock> m_Output;
       Ptr<ToolbarDock> m_Toolbar;
       Ptr<ViewportDock> m_Viewport;
       Ptr<InspectorDock> m_Inspector;
       Ptr<FileViewer> m_FileViewer;
   
       EditorSystem();
       EditorSystem(const EditorSystem&) = delete;
       ~EditorSystem();
   
       void drawDefaultUI(float deltaMilliseconds);
   
       void showDocumentation(const String& name, const sol::table& table);
   
       Variant saveAll(const Event* event);
       Variant autoSave(const Event* event);
       Variant saveBeforeQuit(const Event* event);
       Variant createNewScene(const Event* event);
       Variant createNewMaterial(const Event* event);
   
   public:
       static EditorSystem* GetSingleton();
   
       bool initialize(const JSON::json& systemData) override;
       void update(float deltaMilliseconds) override;
   
       void pushRegularFont() { ImGui::PushFont(m_EditorFont); }
       void pushBoldFont() { ImGui::PushFont(m_EditorFontBold); }
       void pushItalicFont() { ImGui::PushFont(m_EditorFontItalic); }
       void pushMonospaceFont() { ImGui::PushFont(m_EditorFontMonospace); }
   
       void popFont() { ImGui::PopFont(); }
   
       ImColor getWarningColor() const { return ColorToImColor((Color)ColorPresets::LightYellow); }
       ImColor getFatalColor() const { return ColorToImColor((Color)ColorPresets::IndianRed); }
       ImColor getSuccessColor() const { return ColorToImColor((Color)ColorPresets::LimeGreen); }
       ImColor getNormalColor() const { return ColorToImColor((Color)ColorPresets::White); }
   };
