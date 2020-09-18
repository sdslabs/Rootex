#pragma once

#include "common/common.h"

/// Data to be sent in a vertex
struct VertexData
{
	Vector3 m_Position;
	Vector3 m_Normal;
	Vector2 m_TextureCoord;
	Vector3 m_Tangent = { 0.0f, 0.0f, 0.0f };
	Vector3 m_Bitangent = { 0.0f, 0.0f, 0.0f };
};

struct UIVertexData
{
	Vector2 m_Position;
	char m_Color[4];
	Vector2 m_TextureCoord;
};
