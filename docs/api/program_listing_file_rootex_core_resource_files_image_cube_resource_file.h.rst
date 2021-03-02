
.. _program_listing_file_rootex_core_resource_files_image_cube_resource_file.h:

Program Listing for File image_cube_resource_file.h
===================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_resource_files_image_cube_resource_file.h>` (``rootex\core\resource_files\image_cube_resource_file.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "renderer/texture.h"
   #include "resource_file.h"
   
   class ImageCubeResourceFile : public ResourceFile
   {
       Ptr<TextureCube> m_ImageTexture;
   
       explicit ImageCubeResourceFile(const FilePath& path);
   
       friend class ResourceLoader;
   
   public:
       explicit ImageCubeResourceFile(const ImageCubeResourceFile&) = delete;
       explicit ImageCubeResourceFile(const ImageCubeResourceFile&&) = delete;
       ~ImageCubeResourceFile() = default;
   
       void reimport() override;
   
       const TextureCube* getTexture() const { return m_ImageTexture.get(); }
   };
