#pragma once

#include "common/types.h"

struct VSConstantBuffer
{
	Matrix m_M;
	Matrix m_V;
	Matrix m_P;
};

struct PSConstantBuffer
{
	Color m_Colors[6];
};
