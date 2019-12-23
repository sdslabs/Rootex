#pragma once

#include <DirectXMath.h>

struct VSConstantBuffer
{
	DirectX::XMMATRIX m_MVP;
};

struct PSConstantBuffer
{
	DirectX::XMFLOAT4 m_Colors[6];
};
