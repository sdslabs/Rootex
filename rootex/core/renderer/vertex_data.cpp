#include "vertex_data.h"

Vector<VertexData> MakeTriangleVertices()
{
	Vector<VertexData> result;
	
	VertexData a;
	a.pos = { -0.5f, 0.0f, 0.0f };
	VertexData b;
	b.pos = { 0.0f, +0.5f, 0.0f };
	VertexData c;
	c.pos = { +0.5f, 0.0f, 0.0f };
	
	result.push_back(a);
	result.push_back(b);
	result.push_back(c);

	return result;
}

Vector<unsigned short> MakeTriangleIndices()
{
	Vector<unsigned short> result;

	result.push_back(0);
	result.push_back(1);
	result.push_back(2);

	return result;
}
