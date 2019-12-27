#pragma once
#include <vector>

template <typename T>
struct IndexTriangleList
{
	std::vector<T> vertices;
	std::vector<unsigned short> indices;
};