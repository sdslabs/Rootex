#pragma once

#include "common/types.h"

struct VSConstantBuffer
{
	Matrix m_M;
	Matrix m_V;
	Matrix m_P;
};

struct PointLightInfo
{
	// TODO: Get rid of all these magic numbers
	Color ambientColor = { 0.05f, 0.05f, 0.05f, 1.0f };
	Color diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diffuseIntensity = 2.0f;
	float attConst = 1.0f;
	float attLin = 0.045f;
	float attQuad = 0.0075f;
	Vector3 lightPos = { 0.0f, 0.0f, 0.0f };
	float range = 10;
};

struct DirectionalLightInfo
{
	Vector3 direction = { 1.0f, 0.0f, 0.0f };
	float diffuseIntensity = 2.0f;
	Color ambientColor = { 0.05f, 0.05f, 0.05f, 1.0f };
	Color diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
};

struct MaterialInfo
{
	Color materialColor = { 0.7f, 0.7f, 0.0f, 1.0f };
	float specularIntensity = 2.0f;
	float specPow = 30.0f;
	float pad1[2];
};

struct Lights
{
	int pointLightCount = 1;
	float pad[3];
	PointLightInfo pointLightInfos[4];
	int directionalLightPresent = 0;
	float pad2[3];
	DirectionalLightInfo directionalLightInfo;
};

struct PSConstantBuffer
{
	Lights lights;
	MaterialInfo material;
};
