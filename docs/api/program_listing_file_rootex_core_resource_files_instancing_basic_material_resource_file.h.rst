
.. _program_listing_file_rootex_core_resource_files_instancing_basic_material_resource_file.h:

Program Listing for File instancing_basic_material_resource_file.h
==================================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_resource_files_instancing_basic_material_resource_file.h>` (``rootex\core\resource_files\instancing_basic_material_resource_file.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "resource_file.h"
   #include "renderer/shader.h"
   #include "basic_material_resource_file.h"
   #include "image_resource_file.h"
   #include "core/renderer/vertex_data.h"
   
   class InstancingBasicMaterialResourceFile : public BasicMaterialResourceFile
   {
   private:
       static inline Ptr<Shader> s_Shader;
   
   public:
       static void Load();
       static void Destroy();
   
       explicit InstancingBasicMaterialResourceFile(const FilePath& path);
       ~InstancingBasicMaterialResourceFile() = default;
   
       const Shader* getShader() const override { return s_Shader.get(); };
   
       void bindShader() override;
   };
