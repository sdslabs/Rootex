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
	float fogFactor : FOG;
	float3 tangent : TANGENT;
	float4 color : COLOR;
	float3 decalRight : TEXCOORD1;
	float3 decalForward : TEXCOORD2;
	float3 decalUp : TEXCOORD3;
	float3 decalHalfScale : TEXCOORD4;
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

float4 main(DecalPixelInputType input)
    : SV_TARGET
{
	float4 materialColor = ShaderTexture.Sample(SampleType, input.tex) * material.color * input.color;
	float4 finalColor = materialColor;

	float depth = DepthTexture.Sample(SampleType, input.tex).r;
	finalColor.rgb = float3(depth, depth, depth);

	// finalColor = float4(depth, depth, depth, 0.0f);

	// finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	clip(finalColor.a - 0.0001f);

	return finalColor;
}
