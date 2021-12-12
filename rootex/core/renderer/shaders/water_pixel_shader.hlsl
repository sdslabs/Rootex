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

float2 rand(float2 co)
{
	float comp1 = sin(dot(co.xy, float2(12.9898, 78.233)));
	float comp2 = sin(dot(co.xy, float2(13.9838, 34.453)));
	return frac(float2(comp1, comp2));
}

float4 main(PixelInputType input)
    : SV_TARGET
{
	float2 uv = input.tex;
	float time = timeMs / 1000;
	time += 50.0f;
	float minDistance = 100.;
	for (int i = 0; i < 30; i++)
	{
		float2 random = rand(float2(i, i));
		float2 move;
		if (i % 2 == 0)
			move = 0.5 * sin(random * time * 0.21) + 0.5;
		else
			move = 0.5 * sin(-random * time * 0.21) + 0.5;

		float dist = length(uv - move);
		if (dist < minDistance)
		{
			minDistance = dist;
		}
	}
	float3 col = float3(minDistance, minDistance, minDistance);
	col.b += 0.588;
	col.r += 0.043;
	col.g += 0.49;

	//float4 refractColor = texture(iChannel0, uv);
	//col=vec3(0.,0.,0.);
	//refractColor=vec4(0.,0.,0.,0.);
	//float4 color = mix(float4(col, 1.0), refractColor, 0.13);

	return float4(col.r, col.g, col.b, 1.0f);
}
