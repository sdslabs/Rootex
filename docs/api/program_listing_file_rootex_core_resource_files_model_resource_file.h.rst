
.. _program_listing_file_rootex_core_resource_files_model_resource_file.h:

Program Listing for File model_resource_file.h
==============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_resource_files_model_resource_file.h>` (``rootex\core\resource_files\model_resource_file.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "resource_file.h"
   #include "basic_material_resource_file.h"
   
   class ResourceFile;
   class Mesh;
   
   class ModelResourceFile : public ResourceFile
   {
       explicit ModelResourceFile(const FilePath& path);
   
       Vector<Pair<Ref<BasicMaterialResourceFile>, Vector<Mesh>>> m_Meshes;
   
       friend class ResourceLoader;
   
   public:
       explicit ModelResourceFile(const ModelResourceFile&) = delete;
       explicit ModelResourceFile(const ModelResourceFile&&) = delete;
       ~ModelResourceFile() = default;
   
       void reimport() override;
   
       Vector<Pair<Ref<BasicMaterialResourceFile>, Vector<Mesh>>>& getMeshes() { return m_Meshes; }
       int getMaterialCount() { return m_Meshes.size(); }
       Ref<BasicMaterialResourceFile> getMaterialAt(int i) { return m_Meshes[i].first; }
       Vector<Mesh>& getMeshesOfMaterialAt(int i) { return m_Meshes[i].second; }
   };
