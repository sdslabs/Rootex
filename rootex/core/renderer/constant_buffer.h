#pragma once

#include "common/common.h"

/// Model, View, Projection matrices to be bound as the constant buffers in Vertex Shaders
struct VSConstantBuffer
{
	Matrix m_M;
	Matrix m_V;
	Matrix m_P;
};

/// Used to bind a point light to the Pixel shader
struct PointLightInfo
{
	Color ambientColor = { 0.05f, 0.05f, 0.05f, 1.0f };
	Color diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diffuseIntensity = 2.0f;
	/// attenuation = 1/(attConst + attLin * r + attQuad * r * r)
	float attConst = 1.0f;
	/// attenuation = 1/(attConst + attLin * r + attQuad * r * r)
	float attLin = 0.045f;
	/// attenuation = 1/(attConst + attLin * r + attQuad * r * r)
	float attQuad = 0.0075f;
	/// is filled with the TransformComponent while rendering
	Vector3 lightPos = { 0.0f, 0.0f, 0.0f };
	/// Lighting effect clipped for distance > range
	float range = 10;
};

/// Used to bind a directional light to the Pixel shader
struct DirectionalLightInfo
{
	Vector3 direction = { 1.0f, 0.0f, 0.0f };
	float diffuseIntensity = 2.0f;
	Color ambientColor = { 0.05f, 0.05f, 0.05f, 1.0f };
	Color diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
};

/// Used to bind a spot light to the Pixel shader
struct SpotLightInfo
{
	Color ambientColor = { 0.05f, 0.05f, 0.05f, 1.0f };
	Color diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diffuseIntensity = 2.0f;
	float attConst = 1.0f;
	float attLin = 0.045f;
	float attQuad = 0.0075f;
	Vector3 lightPos = { 0.0f, 0.0f, 0.0f };
	float range = 10;
	/// direction of axis of light cone
	Vector3 direction;
	/// increasing spot increases the angular attenuation wrt axis
	float spot;
	/// Lighting effect clipped for angle > angleRange
	float angleRange;
	float pad[3];
};

/// Lighting properties of a material
struct MaterialInfo
{
	/// Describes brightness of specular spot, high for metallic material
	float specularIntensity = 2.0f;
	/// Describes angular fall-off of specular spot, high for metallic material
	float specPow = 30.0f;
	float pad1[2];
};

/// encapsulates all the types of lights offered, to bind them in the Pixel Shader
struct Lights
{
	int pointLightCount = 0;
	float pad[3];
	PointLightInfo pointLightInfos[4];
	int directionalLightPresent = 0;
	float pad2[3];
	DirectionalLightInfo directionalLightInfo;
	int spotLightCount = 0;
	float pad3[3];
	SpotLightInfo spotLightInfos[4];
};

/// Pixel Shader constant buffer for material affected by lighting
struct PSDiffuseConstantBuffer
{
	Lights lights;
	MaterialInfo material;
};

/// Pixel Shader constant buffer for material not affected by lighting and single color
struct PSSolidConstantBuffer
{
	Color color;
};