
.. _program_listing_file_rootex_core_renderer_materials_particles_material.h:

Program Listing for File particles_material.h
=============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_materials_particles_material.h>` (``rootex\core\renderer\materials\particles_material.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "renderer/material.h"
   
   class Texture;
   
   class ParticlesMaterial : public Material
   {
       ParticlesShader* m_ParticlesShader;
       Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;
   
       ImageResourceFile* m_DiffuseImageFile;
       ImageResourceFile* m_NormalImageFile;
       ImageResourceFile* m_SpecularImageFile;
   
       bool m_IsLit;
       bool m_IsNormal;
       float m_SpecularIntensity;
       float m_SpecularPower;
       float m_Reflectivity;
       float m_RefractionConstant;
       float m_Refractivity;
       bool m_IsAffectedBySky;
   
       void setPSConstantBuffer(const PSParticlesConstantBufferMaterial& constantBuffer);
   
   public:
       const static inline String s_MaterialName = "ParticlesMaterial";
       enum class VertexConstantBufferType
       {
           Model,
           End
       };
       enum class PixelConstantBufferType
       {
           Material,
           End
       };
   
       ParticlesMaterial() = delete;
       ParticlesMaterial(bool isAlpha, const String& imagePath, const String& normalImagePath, const String& specularImagePath, bool isNormal, bool isLit, float specularIntensity, float specularPower, float reflectivity, float refractionConstant, float refractivity, bool affectedBySky);
       ~ParticlesMaterial() = default;
   
       void setDiffuseTexture(ImageResourceFile* image);
       void setNormalTexture(ImageResourceFile* image);
       void setSpecularTexture(ImageResourceFile* image);
       void removeNormal();
       void setSpecularIntensity(float specIntensity) { m_SpecularIntensity = specIntensity; }
       void setSpecularPower(float specPower) { m_SpecularPower = specPower; }
   
       static Material* CreateDefault();
       static Material* Create(const JSON::json& materialData);
   
       virtual ID3D11ShaderResourceView* getPreview() override;
   
       void bind() override;
       JSON::json getJSON() const override;
   
       void draw() override;
   };
