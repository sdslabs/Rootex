
.. _program_listing_file_rootex_core_renderer_shaders_register_locations_pixel_shader.h:

Program Listing for File register_locations_pixel_shader.h
==========================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_shaders_register_locations_pixel_shader.h>` (``rootex\core\renderer\shaders\register_locations_pixel_shader.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #ifndef REGISTER_LOCATIONS_PIXEL_SHADER_H
   #define REGISTER_LOCATIONS_PIXEL_SHADER_H
   
   #define CONCAT(a, b) a##b
   
   #define PER_SCENE_PS_CPP 1
   #define PER_SCENE_PS_HLSL CONCAT(b, PER_SCENE_PS_CPP)
   #define PER_FRAME_PS_CPP 2
   #define PER_FRAME_PS_HLSL CONCAT(b, PER_FRAME_PS_CPP)
   #define PER_OBJECT_PS_CPP 3
   #define PER_OBJECT_PS_HLSL CONCAT(b, PER_OBJECT_PS_CPP)
   #define PER_MODEL_PS_CPP 4
   #define PER_MODEL_PS_HLSL CONCAT(b, PER_MODEL_PS_CPP)
   
   #define DIFFUSE_PS_CPP 1
   #define DIFFUSE_PS_HLSL CONCAT(t, DIFFUSE_PS_CPP)
   #define NORMAL_PS_CPP 2
   #define NORMAL_PS_HLSL CONCAT(t, NORMAL_PS_CPP)
   #define SPECULAR_PS_CPP 3
   #define SPECULAR_PS_HLSL CONCAT(t, SPECULAR_PS_CPP)
   #define LIGHTMAP_PS_CPP 4
   #define LIGHTMAP_PS_HLSL CONCAT(t, LIGHTMAP_PS_CPP)
   #define SKY_PS_CPP 5
   #define SKY_PS_HLSL CONCAT(t, SKY_PS_CPP)
   
   #define MAX_STATIC_POINT_LIGHTS 1000
   #define MAX_STATIC_POINT_LIGHTS_AFFECTING_1_OBJECT 10
   #define MAX_DYNAMIC_POINT_LIGHTS 4
   #define MAX_DYNAMIC_SPOT_LIGHTS 4
   
   #endif
