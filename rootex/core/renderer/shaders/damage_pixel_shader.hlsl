#include "register_locations_pixel_shader.h"
Texture2D InputTexture : register(t0);
SamplerState AnisotropicSampler : register(s0);

cbuffer PerFrame : register(CUSTOM_PER_FRAME_PS_HLSL)
{
	float timeMs;
	float deltaTimeMs;
	float2 resolution;
	float2 mouse;
};

struct DamageVSOutput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 main(DamageVSOutput input) : SV_TARGET
{
    float4 rgbl = InputTexture.Sample(AnisotropicSampler, input.tex);
	float2 uv = ( input.tex - 0.5 );
	float time = timeMs * 0.006;
	float freq = 0.5;
	float intensity = max( abs(uv.x), abs(uv.y) );
    intensity = max( intensity, (abs(uv.x) + abs(uv.y)) * 0.66667);
	float sinTime = abs( sin( time ) );
	float div = ( sinTime + 1.0 );
	float3 col = float3( ( (intensity) * sinTime + rgbl.r ) / div, rgbl.g / div, rgbl.b / div);
    rgbl = ( float4( col, 1. ) );

    return rgbl;
}