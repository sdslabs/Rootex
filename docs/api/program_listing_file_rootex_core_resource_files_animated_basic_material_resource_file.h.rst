
.. _program_listing_file_rootex_core_resource_files_animated_basic_material_resource_file.h:

Program Listing for File animated_basic_material_resource_file.h
================================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_resource_files_animated_basic_material_resource_file.h>` (``rootex\core\resource_files\animated_basic_material_resource_file.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "resource_file.h"
   #include "renderer/shader.h"
   #include "basic_material_resource_file.h"
   #include "image_resource_file.h"
   
   class AnimatedBasicMaterialResourceFile : public BasicMaterialResourceFile
   {
   private:
       static inline Ptr<Shader> s_Shader;
   
       Microsoft::WRL::ComPtr<ID3D11Buffer> m_BonesVSCB;
   
   public:
       static void Load();
       static void Destroy();
   
       explicit AnimatedBasicMaterialResourceFile(const FilePath& path);
       ~AnimatedBasicMaterialResourceFile() = default;
   
       void uploadAnimationBuffer(const PerModelAnimationVSCBData& animationBuffer);
   
       const Shader* getShader() const override { return s_Shader.get(); };
   
       void bindShader() override;
       void bindVSCB() override;
   
       void reimport() override;
   };
