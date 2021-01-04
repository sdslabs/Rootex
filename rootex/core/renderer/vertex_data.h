#pragma once

#include "common/common.h"

/// Data to be sent in a vertex
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
