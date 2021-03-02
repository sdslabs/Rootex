
.. _program_listing_file_rootex_core_renderer_shaders_register_locations_vertex_shader.h:

Program Listing for File register_locations_vertex_shader.h
===========================================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_shaders_register_locations_vertex_shader.h>` (``rootex\core\renderer\shaders\register_locations_vertex_shader.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #define CONCAT(a, b) a##b
   
   #define PER_CAMERA_CHANGE_VS_CPP 1
   #define PER_FRAME_VS_CPP 2
   #define PER_OBJECT_VS_CPP 3
   #define BONES_VS_CPP 4
   
   #define PER_OBJECT_VS_HLSL CONCAT(b, PER_OBJECT_VS_CPP)
   #define PER_FRAME_VS_HLSL CONCAT(b, PER_FRAME_VS_CPP)
   #define PER_CAMERA_CHANGE_VS_HLSL CONCAT(b, PER_CAMERA_CHANGE_VS_CPP)
   #define BONES_VS_HLSL CONCAT(b, BONES_VS_CPP)
