
.. _program_listing_file_editor_gui_file_viewer.h:

Program Listing for File file_viewer.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_editor_gui_file_viewer.h>` (``editor\gui\file_viewer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   
   #include "event_manager.h"
   #include "resource_file.h"
   #include "audio_player.h"
   #include "image_viewer.h"
   #include "text_viewer.h"
   #include "material_viewer.h"
   
   class FileViewer
   {
       bool m_IsFileOpened;
       bool m_IsEventJustReceived;
       FilePath m_OpenFilePath;
   
       AudioPlayer m_AudioPlayer;
       ImageViewer m_ImageViewer;
       TextViewer m_TextViewer;
       MaterialViewer m_MaterialViewer;
   
       ResourceFile* m_OpenFile;
   
       void drawFileInfo();
   
       Variant openFile(const Event* event);
   
   public:
       FileViewer();
       FileViewer(FileViewer&) = delete;
       ~FileViewer() = default;
   
       void draw(float deltaMilliseconds);
   };
