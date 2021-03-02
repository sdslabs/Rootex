
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
       Vector3 m_Position;
       Vector3 m_Normal;
       Vector2 m_TextureCoord;
       Vector3 m_Tangent = { 0.0f, 0.0f, 0.0f };
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
       Vector2 m_Position;
       char m_Color[4];
       Vector2 m_TextureCoord;
   };
   
   struct AnimatedVertexData : public VertexData
   {
       int m_BoneIndices[4];
       Vector4 m_BoneWeights;
   };
   
   struct FXAAData
   {
       Vector3 m_Position;
       Vector2 m_Texturecoord;
   };
