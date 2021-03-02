
.. _program_listing_file_editor_gui_image_viewer.h:

Program Listing for File image_viewer.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_editor_gui_image_viewer.h>` (``editor\gui\image_viewer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   
   #include "rootex/core/renderer/texture.h"
   #include "resource_loader.h"
   
   class ImageViewer
   {
       ImageResourceFile* m_ImageResourceFile;
       const float m_ZoomSliderWidth = 40.0f;
       const float m_ZoomSliderHeight = 500.0f;
       const float m_MaxZoom = 3.0f;
       const float m_MinZoom = 0.0f;
       float m_Zoom = 1.0f;
   
       void drawFileInfo();
   
   public:
       ResourceFile* load(const FilePath& filePath);
       void unload();
       void draw(float deltaMilliseconds);
   };
