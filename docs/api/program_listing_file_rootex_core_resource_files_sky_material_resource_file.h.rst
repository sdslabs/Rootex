
.. _program_listing_file_rootex_core_resource_files_sky_material_resource_file.h:

Program Listing for File sky_material_resource_file.h
=====================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_resource_files_sky_material_resource_file.h>` (``rootex\core\resource_files\sky_material_resource_file.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "resource_file.h"
   #include "renderer/shader.h"
   #include "material_resource_file.h"
   #include "image_cube_resource_file.h"
   
   class SkyMaterialResourceFile : public MaterialResourceFile
   {
   private:
       static inline Ptr<Shader> s_Shader;
       static inline Microsoft::WRL::ComPtr<ID3D11SamplerState> s_Sampler;
   
       SkyMaterialData m_MaterialData;
   
       Ref<ImageCubeResourceFile> m_SkyFile;
   
       Microsoft::WRL::ComPtr<ID3D11Buffer> m_VSCB;
   
   public:
       static void Load();
       static void Destroy();
   
       explicit SkyMaterialResourceFile(const FilePath& path);
       ~SkyMaterialResourceFile() = default;
   
       void setSky(Ref<ImageCubeResourceFile> sky);
   
       const Shader* getShader() const override { return s_Shader.get(); };
       Vector<Ref<GPUTexture>> getTextures() const override { return Vector<Ref<GPUTexture>> {}; };
   
       void bindShader() override;
       void bindTextures() override;
       void bindSamplers() override;
       void bindVSCB() override;
       void bindPSCB() override;
   
       JSON::json getJSON() const override;
       ID3D11ShaderResourceView* getPreview() const override;
   
       void reimport() override;
       bool save() override;
       void draw() override;
   };
