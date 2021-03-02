
.. _program_listing_file_rootex_core_resource_files_font_resource_file.h:

Program Listing for File font_resource_file.h
=============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_resource_files_font_resource_file.h>` (``rootex\core\resource_files\font_resource_file.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "resource_file.h"
   
   #include "SpriteFont.h"
   
   class FontResourceFile : public ResourceFile
   {
       explicit FontResourceFile(const FilePath& path);
   
       Ref<DirectX::SpriteFont> m_Font;
   
       friend class ResourceLoader;
   
   public:
       explicit FontResourceFile(const FontResourceFile&) = delete;
       explicit FontResourceFile(const FontResourceFile&&) = delete;
       ~FontResourceFile() = default;
   
       void reimport() override;
   
       Ref<DirectX::SpriteFont> getFont() const { return m_Font; }
   };
