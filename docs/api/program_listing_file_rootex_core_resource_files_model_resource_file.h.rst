
.. _program_listing_file_rootex_core_resource_files_model_resource_file.h:

Program Listing for File model_resource_file.h
==============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_resource_files_model_resource_file.h>` (``rootex\core\resource_files\model_resource_file.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "resource_file.h"
   #include "renderer/mesh.h"
   
   class Material;
   
   class ModelResourceFile : public ResourceFile
   {
       explicit ModelResourceFile(const FilePath& path);
   
       Vector<Pair<Ref<Material>, Vector<Mesh>>> m_Meshes;
   
       friend class ResourceLoader;
   
   public:
       explicit ModelResourceFile(const ModelResourceFile&) = delete;
       explicit ModelResourceFile(const ModelResourceFile&&) = delete;
       ~ModelResourceFile() = default;
   
       void reimport() override;
   
       Vector<Pair<Ref<Material>, Vector<Mesh>>>& getMeshes() { return m_Meshes; }
   };
