
.. _program_listing_file_editor_gui_toolbar_dock.h:

Program Listing for File toolbar_dock.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_editor_gui_toolbar_dock.h>` (``editor\gui\toolbar_dock.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "event_manager.h"
   
   class ToolbarDock
   {
       EventBinder<ToolbarDock> m_Binder;
   
       float m_LastUpdateTime = 0.0f;
   
   public:
       struct ToolbarDockSettings
       {
           bool m_IsActive = true;
           bool m_InEditorPlaying = false;
       };
   
   private:
       ToolbarDockSettings m_ToolbarDockSettings;
       Vector<float> m_FPSRecords;
       unsigned int m_FPSRecordsPoolSize = 100;
       String m_StartPlayingScene;
   
       Variant disablePlayInEditor(const Event* e);
   
   public:
       ToolbarDock();
       ToolbarDock(ToolbarDock&) = delete;
       ~ToolbarDock() = default;
   
       void draw(float deltaMilliseconds);
   
       ToolbarDockSettings& getSettings() { return m_ToolbarDockSettings; }
       void setActive(bool enabled) { m_ToolbarDockSettings.m_IsActive = enabled; }
   };
