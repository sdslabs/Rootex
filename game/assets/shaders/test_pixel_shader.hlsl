#include "rootex/core/renderer/shaders/register_locations_pixel_shader.h"

TextureCube SkyTexture : register(SKY_PS_HLSL);
SamplerState SampleType : register(SAMPLER_PS_HLSL);

cbuffer PerFrame : register(CUSTOM_PER_FRAME_PS_HLSL)
{
    float timeMs;
    float deltaTimeMs;
    float2 resolution;
    float2 mouse;
};

Texture2D Texture0 : register(CUSTOM_TEXTURE_0_PS_HLSL);

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

float4 main(PixelInputType input) : SV_TARGET
{
	float4 color1 = { 1.0f, 0.3f, 0.3f, 1.0f };
	float4 color2 = { 0.3f, 1.0f, 0.3f, 1.0f };
	
	float d1 = length(mouse * resolution - input.screenPosition.xy);
	float d2 = 0.7f * d1;
	
	float4 finalColor = 100.0f * (color1 / d1 + color2 / d2) * Texture0.Sample(SampleType, input.tex);
	finalColor.a = 1.0f;
	
	return finalColor * input.color;
}
