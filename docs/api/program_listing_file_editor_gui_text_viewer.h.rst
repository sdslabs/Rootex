
.. _program_listing_file_editor_gui_text_viewer.h:

Program Listing for File text_viewer.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_editor_gui_text_viewer.h>` (``editor\gui\text_viewer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   
   #include "resource_loader.h"
   
   class TextViewer
   {
       TextResourceFile* m_TextResourceFile;
   
       void drawFileInfo();
   
   public:
       ResourceFile* load(const FilePath& filePath);
       void unload();
       void draw(float deltaMilliseconds);
   };
