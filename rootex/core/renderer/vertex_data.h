#pragma once

#include "common/common.h"

/// Data to be sent in a vertex
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

struct GodRaysData
{
	Vector3 position;
	Vector2 texturecoord;
};
