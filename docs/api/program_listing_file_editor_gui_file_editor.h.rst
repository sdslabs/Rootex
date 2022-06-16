
.. _program_listing_file_editor_gui_file_editor.h:

Program Listing for File file_editor.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_editor_gui_file_editor.h>` (``editor\gui\file_editor.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "event_manager.h"
   
   #include "ImGuiColorTextEdit/TextEditor.h"
   
   class FileEditor
   {
       EventBinder<FileEditor> m_Binder;
   
       TextEditor m_Editor;
   
       String m_EditFilePath;
   
       Variant editFile(const Event* e);
   
   public:
       FileEditor();
   
       void draw(float deltaMilliseconds);
   };
