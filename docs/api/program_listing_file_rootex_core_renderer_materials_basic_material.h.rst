
.. _program_listing_file_rootex_core_renderer_materials_basic_material.h:

Program Listing for File basic_material.h
=========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_materials_basic_material.h>` (``rootex\core\renderer\materials\basic_material.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "renderer/material.h"
   
   class Texture;
   
   class BasicMaterial : public Material
   {
       BasicShader* m_BasicShader;
   
   protected:
       Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;
   
       ImageResourceFile* m_DiffuseImageFile;
       ImageResourceFile* m_NormalImageFile;
       ImageResourceFile* m_SpecularImageFile;
       ImageResourceFile* m_LightmapImageFile;
   
       bool m_IsLit;
       bool m_IsNormal;
       Color m_Color;
       float m_SpecularIntensity;
       float m_SpecularPower;
       float m_Reflectivity;
       float m_RefractionConstant;
       float m_Refractivity;
       bool m_IsAffectedBySky;
   
       void setPSConstantBuffer(const PSDiffuseConstantBufferMaterial& constantBuffer);
       void setVSConstantBuffer(const VSDiffuseConstantBuffer& constantBuffer);
   
   public:
       const static inline String s_MaterialName = "BasicMaterial";
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
   
       BasicMaterial() = delete;
       BasicMaterial(bool isAlpha, const String& imagePath, const String& normalImagePath, const String& specularImagePath, const String& lightmapImagePath, bool isNormal, Color color, bool isLit, float specularIntensity, float specularPower, float reflectivity, float refractionConstant, float refractivity, bool affectedBySky);
       ~BasicMaterial() = default;
   
       void setColor(const Color& color) { m_Color = color; };
       void setDiffuseTexture(ImageResourceFile* image);
       void setNormalTexture(ImageResourceFile* image);
       void setSpecularTexture(ImageResourceFile* image);
       void setLightmapTexture(ImageResourceFile* image);
       void removeNormal();
       void setSpecularIntensity(float specIntensity) { m_SpecularIntensity = specIntensity; }
       void setSpecularPower(float specPower) { m_SpecularPower = specPower; }
   
       static Material* CreateDefault();
       static Material* Create(const JSON::json& materialData);
   
       virtual ID3D11ShaderResourceView* getPreview() override;
   
       virtual void bind() override;
       JSON::json getJSON() const override;
   
       void draw() override;
   };
