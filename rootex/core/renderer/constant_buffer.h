#pragma once

#include "common/types.h"

struct VSConstantBuffer
{
	Matrix m_M;
	Matrix m_V;
	Matrix m_P;
};

struct LightInfo
{
	Color ambientColor = { 0.05f, 0.05f, 0.05f, 1.0f };
	Color diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diffuseIntensity = 2.0f;
	float attConst = 1.0f;
	float attLin = 0.045f;
	float attQuad = 0.0075f;
	Vector3 lightPos = { 0.0f, 0.0f, +2.0f };
	float pad2;
};
struct MaterialInfo
{
	Color materialColor = { 0.7f, 0.7f, 0.0f, 1.0f };
	float specularIntensity = 0.6f;
	float specPow = 30.0f;
	float pad1[2];
};

struct PSConstantBuffer
{
	struct Lights
	{
		int lightCount = 2;
		float pad[3];
		LightInfo lightInfos[4];
	} lights;
	MaterialInfo material;
};
