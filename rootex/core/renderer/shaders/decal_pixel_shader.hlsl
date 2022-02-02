#include "register_locations_pixel_shader.h"
#include "decal_material.hlsli"

Texture2D ShaderTexture : register(DIFFUSE_PS_HLSL);
Texture2D DepthTexture : register(DEPTH_TEXTURE_PS_HLSL);
SamplerState SampleType : register(SAMPLER_PS_HLSL);

cbuffer CBuf : register(PER_OBJECT_PS_HLSL)
{
	DecalMaterial material;
};

cbuffer CBuf : register(PER_DECAL_PS_HLSL)
{
	float3 decalRight;
	float3 decalForward;
	float3 decalUp;
	float3 decalHalfScale;
	float3 decalViewspacePosition;
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
	return depthLinearizeMul / (depthLinearizeAdd - screenDepth);
}

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
	float3 scale = decalHalfScale;
	float3 p = pos - decalPos;
	bool inX = (abs(dot(p, decalRight)) <= scale.x);
	bool inY = (abs(dot(p, decalForward)) <= scale.y);
	bool inZ = (abs(dot(p, decalUp)) <= scale.z);
	return inX && inY && inZ;
}

struct PixelInputType
{
	float4 screenPosition : SV_POSITION;
	float3 normal : NORMAL;
	float4 worldPosition : POSITION;
	float2 tex : TEXCOORD0;
	float fogFactor : FOG;
	float3 tangent : TANGENT;
	float4 color : COLOR;
};

float4 main(PixelInputType input)
    : SV_TARGET
{
	uint3 index = uint3(input.screenPosition.x, input.screenPosition.y, 0);
	float depth = DepthTexture.Load(index).r;

	float viewspaceDepth = ScreenSpaceToViewSpaceDepth(depth);
	float3 viewPos = ScreenSpaceToViewSpacePosition(input.screenPosition.xy, viewspaceDepth);
	float3 viewspacePosition = float3(viewPos.x, -viewPos.y, -viewPos.z);

	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	if (IsPointInDecalBounds(viewspacePosition, decalViewspacePosition, decalRight, decalForward, decalUp, decalHalfScale))
	{
		float3 r = viewspacePosition - decalViewspacePosition;
		float localX = dot(r, decalRight) / (2 * decalHalfScale.x) + 0.5f;
		float localY = -dot(r, decalUp) / (2 * decalHalfScale.y) + 0.5f;
		finalColor = ShaderTexture.Sample(SampleType, float2(localX, localY)) * material.color * input.color;
	}

	return finalColor;
}
