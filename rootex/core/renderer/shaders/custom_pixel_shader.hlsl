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
    return Texture0.Sample(SampleType, input.tex);
}
