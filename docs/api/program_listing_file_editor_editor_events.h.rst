
.. _program_listing_file_editor_editor_events.h:

Program Listing for File editor_events.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_editor_editor_events.h>` (``editor\editor_events.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "rootex/core/event.h"
   
   struct EditorEvents
   {
       DEFINE_EVENT(EditorAutoSave);
   
       DEFINE_EVENT(EditorSceneIsClosing);
   
       DEFINE_EVENT(EditorCreateNewScene, String);
   
       DEFINE_EVENT(EditorCreateNewFile, String);
   
       DEFINE_EVENT(EditorOpenFile, String, int);
   
       DEFINE_EVENT(EditorEditFile, String);
   
       DEFINE_EVENT(EditorOpenScene, Scene*);
   
       DEFINE_EVENT(EditorSaveBeforeQuit);
   
       DEFINE_EVENT(EditorSaveAll);
   
       DEFINE_EVENT(EditorReset);
   };
