
.. _program_listing_file_editor_gui_material_viewer.h:

Program Listing for File material_viewer.h
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file_editor_gui_material_viewer.h>` (``editor\gui\material_viewer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   
   #include "resource_file.h"
   #include "core/resource_files/material_resource_file.h"
   
   class MaterialViewer
   {
       Ref<MaterialResourceFile> m_OpenedMaterial;
   
   public:
       Ref<ResourceFile> load(const FilePath& filePath);
       void unload();
   
       void draw(float deltaMilliseconds);
   };
