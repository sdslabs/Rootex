#pragma once

#include "common/common.h"

/// Data to be sent in a vertex
struct VertexData
{
	Vector3 m_Position;
	Vector3 m_Normal;
	Vector2 m_TextureCoord;
};

struct AnimationVertexData
{
	Vector3 m_Position;
	Vector3 m_Normal;
	Vector2 m_TextureCoord;
	Vector4 m_BoneWeights;
	unsigned int m_BoneIndices[4];
};
