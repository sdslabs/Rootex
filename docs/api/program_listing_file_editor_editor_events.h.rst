
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
   
       DEFINE_EVENT(EditorCloseScene);
   
       DEFINE_EVENT(EditorCreateNewScene, String);
   
       DEFINE_EVENT(EditorCreateNewMaterial);
   
       DEFINE_EVENT(EditorOpenFile, String);
   
       DEFINE_EVENT(EditorOpenScene, Scene*);
   
       DEFINE_EVENT(EditorSaveBeforeQuit);
   
       DEFINE_EVENT(EditorSaveAll);
   };
