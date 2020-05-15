#pragma once

#include "common/common.h"

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
	/// Is filled with the TransformComponent while rendering
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
	/// Direction of axis of light cone
	Vector3 direction;
	/// Increasing spot increases the angular attenuation wrt axis
	float spot;
	/// Lighting effect clipped for angle > angleRange
	float angleRange;
	float pad[3];
};

/// Lighting properties of a material
struct PSDiffuseConstantBufferMaterial
{
	Color color;
	int isLit;
	/// Describes brightness of specular spot, high for metallic material
	float specularIntensity = 2.0f;
	/// Describes angular fall-off of specular spot, high for metallic material
	float specularPower = 30.0f;
	float pad;
};

/// Encapsulates all the types of lights offered, to bind them in the Pixel Shader
struct PSDiffuseConstantBufferLights
{
	Vector3 cameraPos;
	int pointLightCount = 0;
	PointLightInfo pointLightInfos[4];
	int directionalLightPresent = 0;
	float pad2[3];
	DirectionalLightInfo directionalLightInfo;
	int spotLightCount = 0;
	float pad3[3];
	SpotLightInfo spotLightInfos[4];
};

/// Pixel Shader constant buffer for material not affected by lighting and single color
struct PSSolidConstantBuffer
{
	Color color;
};

/// Vertex Shader constant buffer for material not affected by lighting
struct VSSolidConstantBuffer
{
	Matrix Model;
	explicit VSSolidConstantBuffer() = delete;
	VSSolidConstantBuffer(const Matrix& model)
	{
		Model = model.Transpose();
	}
};

/// Vertex Shader constant buffer for material affected by lighting
struct VSDiffuseConstantBuffer
{
	Matrix Model;
	Matrix ModelInverseTranspose;
	explicit VSDiffuseConstantBuffer() = delete; // https://stackoverflow.com/a/43694276
	VSDiffuseConstantBuffer(const Matrix& model)
	{
		Model = model.Transpose();
		ModelInverseTranspose = model.Invert();
	}
};
