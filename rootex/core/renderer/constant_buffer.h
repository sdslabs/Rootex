#pragma once

#include <DirectXMath.h>

struct VSConstantBuffer
{
	DirectX::XMMATRIX m_M;
	DirectX::XMMATRIX m_V;
	DirectX::XMMATRIX m_P;
};

struct PSConstantBuffer
{
	DirectX::XMFLOAT4 m_Colors[6];
};
