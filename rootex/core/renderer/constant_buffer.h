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
	Color materialColor = { 0.7f, 0.7f, 0.0f, 1.0f };
	Color ambientColor = { 0.05f, 0.05f, 0.05f, 1.0f };
	Color diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diffuseIntensity = 2.0f;
	float specularIntensity = 0.6f;
	float attConst = 1.0f;
	float attLin = 0.045f;
	float attQuad = 0.0075f;
	float specPow = 30.0f;
	float pad1;
	float pad2;
	Vector3 lightPos = { 0.0f, 0.0f, +2.0f };
	float pad3;
};
