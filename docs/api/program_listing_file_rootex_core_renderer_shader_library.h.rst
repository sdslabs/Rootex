
.. _program_listing_file_rootex_core_renderer_shader_library.h:

Program Listing for File shader_library.h
=========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_shader_library.h>` (``rootex\core\renderer\shader_library.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "shader.h"
   
   class ShaderLibrary
   {
       enum class ShaderType
       {
           Basic,
           Sky,
           Particles,
           Animation,
           FXAA,
           Luma
       };
   
   private:
       static HashMap<ShaderType, Ptr<Shader>> s_Shaders;
   
       static Shader* MakeShader(ShaderType shaderType, const LPCWSTR& vertexPath, const LPCWSTR& pixelPath, const BufferFormat& vertexBufferFormat);
   
   public:
       static void MakeShaders();
       static void DestroyShaders();
   
       static BasicShader* GetBasicShader();
       static ParticlesShader* GetParticlesShader();
       static SkyShader* GetSkyShader();
       static AnimationShader* GetAnimationShader();
       static FXAAShader* GetFXAAShader();
       static LumaShader* GetLumaShader();
   };
