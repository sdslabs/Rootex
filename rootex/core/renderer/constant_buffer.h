#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

struct VSConstantBuffer
{
	DirectX::XMMATRIX m_Transform;
};

struct PSConstantBuffer
{
	struct Color
	{
		float r;
		float g;
		float b;
		float a;
	} m_FaceColors[6];
};
