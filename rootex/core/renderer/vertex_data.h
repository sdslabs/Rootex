#pragma once

#include "common/types.h"

struct VertexData
{
	struct
	{
		float x, y, z;
	} pos;
};

Vector<VertexData> MakeTriangleVertices();
Vector<unsigned short> MakeTriangleIndices();
