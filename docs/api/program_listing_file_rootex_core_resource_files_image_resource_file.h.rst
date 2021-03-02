
.. _program_listing_file_rootex_core_resource_files_image_resource_file.h:

Program Listing for File image_resource_file.h
==============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_resource_files_image_resource_file.h>` (``rootex\core\resource_files\image_resource_file.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "resource_file.h"
   #include "renderer/texture.h"
   
   class ImageResourceFile : public ResourceFile
   {
       Ref<Texture> m_ImageTexture;
   
       explicit ImageResourceFile(const FilePath& path);
   
       friend class ResourceLoader;
   
   public:
       explicit ImageResourceFile(const ImageResourceFile&) = delete;
       explicit ImageResourceFile(const ImageResourceFile&&) = delete;
       ~ImageResourceFile() = default;
   
       void reimport() override;
   
       const Ref<Texture> getTexture() { return m_ImageTexture; }
       unsigned int getWidth() const { return m_ImageTexture->getWidth(); }
       unsigned int getHeight() const { return m_ImageTexture->getHeight(); }
   };
