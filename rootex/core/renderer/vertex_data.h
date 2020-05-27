#pragma once

#include "common/common.h"

/// Data to be sent in a vertex
struct VertexData
{
	Vector3 m_Position;
	Vector3 m_Normal;
	Vector2 m_TextureCoord;
};

struct UIVertexData
{
	Vector2 m_Position;
	uint8_t m_Color;
	Vector2 m_TextureCoord;
};
