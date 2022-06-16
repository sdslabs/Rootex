
.. _program_listing_file_rootex_core_renderer_vertex_data.h:

Program Listing for File vertex_data.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_vertex_data.h>` (``rootex\core\renderer\vertex_data.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   
   struct VertexData
   {
       Vector3 position;
       Vector3 normal;
       Vector2 textureCoord;
       Vector3 tangent;
   };
   
   struct InstanceData
   {
       Matrix transform;
       Matrix inverseTransposeTransform;
       Color color;
   
       InstanceData() = default;
       InstanceData(const Matrix& matrix, const Color& instanceColor)
       {
           transform = matrix;
           inverseTransposeTransform = matrix.Invert().Transpose();
           color = instanceColor;
       }
   };
   
   struct UIVertexData
   {
       Vector2 position;
       char color[4];
       Vector2 textureCoord;
   };
   
   struct AnimatedVertexData : public VertexData
   {
       int boneIndices[4];
       Vector4 boneWeights;
   };
   
   struct FXAAData
   {
       Vector3 position;
       Vector2 texturecoord;
   };
