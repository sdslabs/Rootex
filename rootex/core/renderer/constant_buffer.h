#pragma once

#include "common/common.h"
#include "core/renderer/shaders/register_locations_pixel_shader.h"
#include "core/renderer/shaders/register_locations_vertex_shader.h"

/// Used to bind a point light to the Pixel shader
struct PointLightInfo
{
	Color ambientColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	Color diffuseColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	float diffuseIntensity = 0.0f;
	/// attenuation = 1/(attConst + attLin * r + attQuad * r * r)
	float attConst = 1.0f;
	/// attenuation = 1/(attConst + attLin * r + attQuad * r * r)
	float attLin = 0.045f;
	/// attenuation = 1/(attConst + attLin * r + attQuad * r * r)
	float attQuad = 0.0075f;
	/// Is filled with the TransformComponent while rendering
	Vector3 lightPos = { 0.0f, 0.0f, 0.0f };
	/// Lighting effect clipped for distance > range
	float range = 0.0f;
};

/// Used to bind a directional light to the Pixel shader
struct DirectionalLightInfo
{
	Vector3 direction = { 1.0f, 0.0f, 0.0f };
	float diffuseIntensity = 0.0f;
	Color ambientColor = { 0.05f, 0.05f, 0.05f, 1.0f };
	Color diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
};

/// Used to bind a spot light to the Pixel shader
struct SpotLightInfo
{
	Color ambientColor = { 0.05f, 0.05f, 0.05f, 1.0f };
	Color diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diffuseIntensity = 0.0f;
	float attConst = 1.0f;
	float attLin = 0.045f;
	float attQuad = 0.0075f;
	Vector3 lightPos = { 0.0f, 0.0f, 0.0f };
	float range = 0.0f;
	/// Direction of axis of light cone
	Vector3 direction;
	/// Increasing spot increases the angular attenuation wrt axis
	float spot;
	/// Lighting effect clipped for angle > angleRange
	float angleRange;
	float pad[3];
};

struct StaticLightID
{
	int id;
	Vector3 pad;
};

struct PerModelPSCB
{
	int staticPointsLightsAffectingCount = 0;
	float pad[3];
	StaticLightID staticPointsLightsAffecting[MAX_STATIC_POINT_LIGHTS_AFFECTING_1_OBJECT];
};

struct StaticPointLightsInfo
{
	PointLightInfo pointLightInfos[MAX_STATIC_POINT_LIGHTS];
};

struct LightsInfo
{
	Vector3 cameraPos;
	int pointLightCount = 0;
	PointLightInfo pointLightInfos[MAX_DYNAMIC_POINT_LIGHTS];
	int directionalLightPresent = 0;
	float pad2[3];
	DirectionalLightInfo directionalLightInfo;
	int spotLightCount = 0;
	float pad3[3];
	SpotLightInfo spotLightInfos[MAX_DYNAMIC_SPOT_LIGHTS];
};

/// Constant buffer uploaded once per frame in the PS
struct PerFramePSCB
{
	LightsInfo lights;
	Color fogColor;
};

/// Constant buffer uploaded once per frame in the VS
struct PerFrameVSCB
{
	Matrix view;
	float fogStart;
	float fogEnd;
	float pad[2];
};

/// Constant buffer uploaded at least once per scene load in the PS
struct PerScenePSCB
{
	StaticPointLightsInfo staticLights;
};

/// PS constant buffer used during the FXAA post process.
struct PSFXAACB
{
	/// { 1.0f / screenWidth, 1.0f / screenHeight, 0.0f, 0.0f }
	Vector4 rcpFrame;
};
