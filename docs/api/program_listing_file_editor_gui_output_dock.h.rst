
.. _program_listing_file_editor_gui_output_dock.h:

Program Listing for File output_dock.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_editor_gui_output_dock.h>` (``editor\gui\output_dock.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <iostream>
   #include <sstream>
   
   #include "common/common.h"
   #include "event_manager.h"
   
   class OutputDock
   {
       enum class OutputState : unsigned int
       {
           None = 0,
           General = 1 << 0,
           Warning = 1 << 1,
           Error = 1 << 2,
           All = General | Warning | Error
       };
   
       EventBinder<OutputDock> m_Binder;
   
   public:
       struct OutputDockSettings
       {
           bool m_IsActive = true;
       };
   
   private:
       OutputDockSettings m_OutputDockSettings;
       Vector<Pair<String, String>> m_CaughtOutputs;
       bool m_IsOutputJustCaught = false;
       Variant catchOutput(const Event* event);
   
   public:
       OutputDock();
       OutputDock(OutputDock&) = delete;
       ~OutputDock() = default;
   
       void draw(float deltaMilliseconds);
   
       OutputDockSettings& getSettings() { return m_OutputDockSettings; }
       void setActive(bool enabled) { m_OutputDockSettings.m_IsActive = enabled; }
   };
