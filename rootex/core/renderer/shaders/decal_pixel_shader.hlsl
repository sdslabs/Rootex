#include "register_locations_pixel_shader.h"
#include "basic_material.hlsli"

Texture2D ShaderTexture : register(DIFFUSE_PS_HLSL);
Texture2D NormalTexture : register(NORMAL_PS_HLSL);
Texture2D SpecularTexture : register(SPECULAR_PS_HLSL);
Texture2D LightmapTexture : register(LIGHTMAP_PS_HLSL);
Texture2D DepthTexture : register(DEPTH_TEXTURE_PS_HLSL);
SamplerState SampleType : register(SAMPLER_PS_HLSL);

struct DecalPixelInputType
{
	float4 screenPosition : SV_POSITION;
	float3 normal : NORMAL;
	float4 worldPosition : POSITION;
	float2 tex : TEXCOORD0;
	float3 tangent : TANGENT;
	float4 color : COLOR;
	nointerpolation float3 decalRight : TEXCOORD1;
	nointerpolation float3 decalForward : TEXCOORD2;
	nointerpolation float3 decalUp : TEXCOORD3;
	nointerpolation float3 decalHalfScale : TEXCOORD4;
	nointerpolation float4 decalViewspacePosition : TEXCOORD5;
};

cbuffer CBuf : register(PER_OBJECT_PS_HLSL)
{
	BasicMaterial material;
};

cbuffer CBuf : register(PER_MODEL_PS_HLSL)
{
	int staticPointLightAffectingCount = 0;
	int staticPointsLightsAffecting[MAX_STATIC_POINT_LIGHTS_AFFECTING_1_OBJECT];
};

cbuffer CBuf : register(PER_CAMERA_CHANGE_PS_HLSL)
{
	float2 DepthUnpackConsts;
	float2 Viewport2xPixelSize;
	float2 CameraTanHalfFOV;
}

float ScreenSpaceToViewSpaceDepth(float screenDepth)
{
	float depthLinearizeMul = DepthUnpackConsts.x;
	float depthLinearizeAdd = DepthUnpackConsts.y;

	// Optimised version of "-cameraClipNear / (cameraClipFar - projDepth * (cameraClipFar - cameraClipNear)) * cameraClipFar"

	// Set your depthLinearizeMul and depthLinearizeAdd to:
	// depthLinearizeMul = ( cameraClipFar * cameraClipNear) / ( cameraClipFar - cameraClipNear );
	// depthLinearizeAdd = cameraClipFar / ( cameraClipFar - cameraClipNear );

	return depthLinearizeMul / (depthLinearizeAdd - screenDepth);
}

// from [0, width], [0, height] to [-1, 1], [-1, 1]
float2 ScreenSpaceToClipSpacePositionXY(float2 screenPos)
{
	return screenPos * Viewport2xPixelSize.xy - float2(1.0f, 1.0f);
}

float3 ScreenSpaceToViewSpacePosition(float2 screenPos, float viewspaceDepth)
{
	return float3(CameraTanHalfFOV.xy * viewspaceDepth * ScreenSpaceToClipSpacePositionXY(screenPos), viewspaceDepth);
}

bool IsPointInDecalBounds(float3 pos, float3 decalPos, float3 decalRight, float3 decalForward, float3 decalUp, float3 decalHalfScale)
{
	float3 scale = decalHalfScale * 2.0f;
	float3 p = pos - decalPos;
	bool inX = (abs(dot(p, decalRight)) <= scale.x);
	bool inY = (abs(dot(p, decalForward)) <= scale.y);
	bool inZ = (abs(dot(p, decalUp)) <= scale.z);
	return inX && inY && inZ;
	// return abs(dot(p, decalRight)) <= scale.x && abs(dot(p, decalForward)) <= scale.y && abs(dot(p, decalUp)) <= scale.z;
}

float4 main(DecalPixelInputType input)
    : SV_TARGET
{
	float4 materialColor = ShaderTexture.Sample(SampleType, input.tex) * material.color * input.color;
	float4 finalColor = materialColor;

	uint3 index = uint3(input.screenPosition.x, input.screenPosition.y, 0);
	float depth = DepthTexture.Load(index).r;

	float viewspaceDepth = ScreenSpaceToViewSpaceDepth(depth);
	float3 viewPos = ScreenSpaceToViewSpacePosition(input.screenPosition.xy, viewspaceDepth);
	float3 viewspacePosition = float3(viewPos.x, -viewPos.y, -viewPos.z);
	// float3 viewspacePosition = viewPos;

	finalColor = float4(1.0f, 1.0f, 1.0f, 0.0f);

	float3 decalViewspacePosition = input.decalViewspacePosition.xyz / input.decalViewspacePosition.w;
	if (IsPointInDecalBounds(viewspacePosition, decalViewspacePosition, input.decalRight, input.decalForward, input.decalUp, input.decalHalfScale))
	{
		finalColor.a = 1.0f;
	}

	// finalColor = float4(depth, depth, depth, 0.0f);

	// finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// clip(finalColor.a - 0.0001f);
	// finalColor.rgb = abs(viewspacePosition) / 10.0f;
	// finalColor.rgb = float3(1.0f, 0.0f, 0.0f);

	return finalColor;
}
