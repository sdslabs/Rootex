
.. _program_listing_file_rootex_core_renderer_materials_animated_material.h:

Program Listing for File animated_material.h
============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_materials_animated_material.h>` (``rootex\core\renderer\materials\animated_material.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "renderer/material.h"
   #include "basic_material.h"
   
   class AnimatedMaterial : public BasicMaterial
   {
       AnimationShader* m_AnimationShader;
   
   public:
       const static inline String s_MaterialName = "AnimatedMaterial";
       enum class VertexConstantBufferType
       {
           Model,
           Animation,
           End
       };
   
       AnimatedMaterial() = delete;
       AnimatedMaterial(bool isAlpha, const String& imagePath, const String& normalImagePath, const String& specularImagePath, const String& lightmapPath, bool isNormal, Color color, bool isLit, float specularIntensity, float specularPower, float reflectivity, float refractionConstant, float refractivity, bool affectedBySky);
       ~AnimatedMaterial() = default;
   
       void setVSConstantBuffer(const VSAnimationConstantBuffer& constantBuffer);
   
       static Material* CreateDefault();
       static Material* Create(const JSON::json& materialData);
   
       void bind() override;
   };
