
.. _program_listing_file_rootex_core_renderer_post_processor.h:

Program Listing for File post_processor.h
=========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_post_processor.h>` (``rootex\core\renderer\post_processor.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common.h"
   
   #include "PostProcess.h"
   #include "ASSAO.h"
   
   class CameraComponent;
   
   struct PostProcessingDetails
   {
       bool isPostProcessing = false;
       bool isASSAO = false;
       bool isBloom = false;
       bool isSepia = false;
       bool isMonochrome = false;
       bool isGaussianBlur = false;
       bool isToneMap = false;
       bool isFXAA = false;
       float assaoRadius = 1.2f;
       float assaoDetailShadowStrength = 0.5f;
       int assaoBlurPassCount = 2;
       float assaoFadeOutFrom = 50.0f;
       float assaoFadeOutTo = 300.0f;
       float assaoHorizonAngleThreshold = 0.06f;
       int assaoQualityLevel = 2;
       float assaoShadowClamp = 0.98f;
       float assaoShadowMultiplier = 1.0f;
       float assaoShadowPower = 1.5f;
       float assaoSharpness = 0.98f;
       float assaoAdaptiveQualityLimit = 0.45f;
       float bloomThreshold = 0.8f;
       float bloomSize = 1.0f;
       float bloomBrightness = 1.0f;
       float bloomValue = 1.0f;
       float bloomBase = 1.0f;
       float bloomSaturation = 1.0f;
       float bloomBaseSaturation = 1.0f;
       float gaussianBlurMultiplier = 1.0f;
       float toneMapExposure = 0.0f;
       int toneMapOperator = 0;
       int toneMapTransferFunction = 0;
       float toneMapWhiteNits = 200.0f;
   };
   
   class PostProcess
   {
   public:
       virtual ~PostProcess() = default;
   
       virtual void draw(CameraComponent* camera, ID3D11ShaderResourceView*& nextSource) = 0;
   };
   
   class PostProcessor
   {
       Vector<Ptr<PostProcess>> m_PostProcesses;
   
       Ptr<DirectX::BasicPostProcess> m_BasicPostProcess;
   
       PostProcessor();
       PostProcessor(PostProcessor&) = delete;
       ~PostProcessor() = default;
   
   public:
       static PostProcessor* GetSingleton();
   
       void draw(CameraComponent* camera);
   };
