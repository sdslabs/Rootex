
.. _program_listing_file_rootex_core_renderer_constant_buffer.h:

Program Listing for File constant_buffer.h
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_constant_buffer.h>` (``rootex\core\renderer\constant_buffer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "core/renderer/shaders/register_locations_pixel_shader.h"
   #include "core/renderer/shaders/register_locations_vertex_shader.h"
   
   struct PointLightInfo
   {
       Color ambientColor = { 0.0f, 0.0f, 0.0f, 0.0f };
       Color diffuseColor = { 0.0f, 0.0f, 0.0f, 0.0f };
       float diffuseIntensity = 0.0f;
       float attConst = 1.0f;
       float attLin = 0.045f;
       float attQuad = 0.0075f;
       Vector3 lightPos = { 0.0f, 0.0f, 0.0f };
       float range = 0.0f;
   };
   
   struct DirectionalLightInfo
   {
       Vector3 direction = { 1.0f, 0.0f, 0.0f };
       float diffuseIntensity = 0.0f;
       Color ambientColor = { 0.05f, 0.05f, 0.05f, 1.0f };
       Color diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
   };
   
   struct SpotLightInfo
   {
       Color ambientColor = { 0.05f, 0.05f, 0.05f, 1.0f };
       Color diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
       float diffuseIntensity = 0.0f;
       float attConst = 1.0f;
       float attLin = 0.045f;
       float attQuad = 0.0075f;
       Vector3 lightPos = { 0.0f, 0.0f, 0.0f };
       float range = 0.0f;
       Vector3 direction;
       float spot;
       float angleRange;
       float pad[3];
   };
   
   struct StaticLightID
   {
       int id;
       Vector3 pad;
   };
   
   // Constant buffer uploaded once per model being rendered
   struct PerModelPSCB
   {
       int staticPointsLightsAffectingCount = 0;
       float pad[3];
       StaticLightID staticPointsLightsAffecting[MAX_STATIC_POINT_LIGHTS_AFFECTING_1_OBJECT];
   };
   
   struct PSDiffuseConstantBufferMaterial
   {
       Color color;
       int isLit = 0;
       float specularIntensity = 2.0f;
       float specularPower = 30.0f;
       float reflectivity = 0.0f;
       float refractionConstant = 0.5f;
       float refractivity = 0.0f;
       int affectedBySky = 0;
       int hasNormalMap = 0;
   };
   
   struct PSParticlesConstantBufferMaterial
   {
       int isLit = 0;
       float specularIntensity = 2.0f;
       float specularPower = 30.0f;
       float reflectivity = 0.0f;
       float refractionConstant = 0.5f;
       float refractivity = 0.0f;
       int affectedBySky = 0;
       int hasNormalMap = 0;
   };
   
   struct StaticPointLightsInfo
   {
       PointLightInfo pointLightInfos[MAX_STATIC_POINT_LIGHTS];
   };
   
   struct LightsInfo
   {
       Vector3 cameraPos;
       int pointLightCount = 0;
       PointLightInfo pointLightInfos[MAX_DYNAMIC_POINT_LIGHTS];
       int directionalLightPresent = 0;
       float pad2[3];
       DirectionalLightInfo directionalLightInfo;
       int spotLightCount = 0;
       float pad3[3];
       SpotLightInfo spotLightInfos[MAX_DYNAMIC_SPOT_LIGHTS];
   };
   
   struct PerFramePSCB
   {
       LightsInfo lights;
       Color fogColor;
   };
   
   struct PerFrameVSCB
   {
       Matrix view;
       float fogStart;
       float fogEnd;
       float pad[2];
   };
   
   struct PerLevelPSCB
   {
       StaticPointLightsInfo staticLights;
   };
   
   struct PSSolidConstantBuffer
   {
       Color color;
   };
   
   struct VSSolidConstantBuffer
   {
       Matrix Model;
       explicit VSSolidConstantBuffer() = delete;
       VSSolidConstantBuffer(const Matrix& model)
       {
           Model = model.Transpose();
       }
   };
   
   struct VSDiffuseConstantBuffer
   {
       Matrix Model;
       Matrix ModelInverseTranspose;
       explicit VSDiffuseConstantBuffer() = delete; // https://stackoverflow.com/a/43694276
       VSDiffuseConstantBuffer(const Matrix& model)
       {
           Model = model.Transpose();
           ModelInverseTranspose = model.Invert();
       }
   };
   
   struct VSAnimationConstantBuffer
   {
       Matrix m_BoneTransforms[256];
       explicit VSAnimationConstantBuffer() = delete;
       VSAnimationConstantBuffer(const Vector<Matrix>& transforms)
       {
           for (int i = 0; i < transforms.size(); i++)
           {
               m_BoneTransforms[i] = transforms[i].Transpose();
           }
       }
   };
   
   struct PSFXAACB
   {
       Vector4 rcpFrame;
   };
