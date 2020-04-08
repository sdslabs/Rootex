#pragma once
#include <vector>

/// Encapsulated Vertices and Indices
template <typename T>
struct IndexTriangleList
{
	std::vector<T> vertices;
	std::vector<unsigned short> indices;
};