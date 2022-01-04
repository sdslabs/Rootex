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

float2 hash(float2 p)
{
	p = float2(dot(p, float2(127.1, 311.7)),
	    dot(p, float2(269.5, 183.3)));

	return -1.0 + 2.0 * frac(sin(p) * 43758.5453123);
}

float noise(in float2 p) //simplex noise: https://en.wikipedia.org/wiki/Simplex_noise
	const float K1 = 0.366025404; // (sqrt(3)-1)/2;
	const float K2 = 0.211324865; // (3-sqrt(3))/6;

	float2 i = floor(p + (p.x + p.y) * K1);

	float2 a = p - i + (i.x + i.y) * K2;
	float2 o = step(a.yx, a.xy);
	float2 b = a - o + K2;
	float2 c = a - 1.0 + 2.0 * K2;

	float3 h = max(0.5 - float3(dot(a, a), dot(b, b), dot(c, c)), 0.0);

	float3 n = h * h * h * h * float3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));

	return dot(n, float3(70.0,70.0,70.0));
}

float fbm(in float2 p) //Fractal Brownian Motion: https://en.wikipedia.org/wiki/Fractional_Brownian_motion
{
	float f = 0.0;
	float2x2 m = float2x2(1.6, 1.2, -1.2, 1.6);
	f = 0.5000 * noise(p);
	p = mul(m,p);
	f += 0.2500 * noise(p);
	p = mul(m,p);
	f += 0.1250 * noise(p);
	p = mul(m,p);
	f += 0.0625 * noise(p);
	p = mul(m,p);
	f = 0.5 + 0.5 * f;
	return f;
}

float4 main(PixelInputType input)
    : SV_TARGET
{
	float2 fireMovement = float2(-0.01, -0.5);
	float2 distortionMovement = float2(-0.01, -0.3);
	float normalStrength = 40.0;
	float distortionStrength = 0.1;

	float2 uv = input.tex;
	uv.y = 1 - uv.y;
	float time = timeMs * 0.34 / 1000;

	float2 movingFireMovement = float2(fireMovement.x * time, fireMovement.y * time);
	float2 weight = float2(1.0, 0.5);
	float2 uvWeight = float2(uv.x * weight.x, uv.y * weight.y);
	float2 uvT = uvWeight + movingFireMovement;
	float movingNoise = pow(fbm(8.0 * uvT), 1.0);

	float gradient = pow(1.0 - uv.y, 2.0) * 5.;
	float finalNoise = movingNoise * gradient;
	float3 weightedColor = float3(240.0, 120.0, 0.0) / 255.;

	float3 finalColor = (finalNoise * weightedColor);
	float power = 3; //Maybe As a tweakable factor???
	float alpha =( pow(finalColor.r, power) + pow(finalColor.b, power) + pow(finalColor.g, power) )/ 3.0f;
	return float4(finalColor,alpha);
}

