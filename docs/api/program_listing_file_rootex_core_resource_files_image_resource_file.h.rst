
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
       Ref<GPUTexture> m_GPUImageTexture;
       Ref<CPUTexture> m_CPUImageTexture;
       bool m_IsCPUAccess;
   
       explicit ImageResourceFile(const FilePath& path, bool cpuAccess = false);
   
       void loadCPUTexture();
   
       friend class ResourceLoader;
   
   public:
       explicit ImageResourceFile(const ImageResourceFile&) = delete;
       explicit ImageResourceFile(const ImageResourceFile&&) = delete;
       ~ImageResourceFile() = default;
   
       void reimport() override;
   
       const Ref<GPUTexture> getGPUTexture() { return m_GPUImageTexture; }
       const Ref<CPUTexture> getCPUTexture() { return m_CPUImageTexture; }
   
       unsigned int getWidth() const { return m_GPUImageTexture->getWidth(); }
       unsigned int getHeight() const { return m_GPUImageTexture->getHeight(); }
   
       void setCPUAccess(bool cpuAccess);
       bool isCPUAccess() { return m_IsCPUAccess; };
   
       void uploadCPUTexturetoGPU();
   };
