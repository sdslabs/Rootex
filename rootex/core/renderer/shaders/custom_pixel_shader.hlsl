#include "rootex/core/renderer/shaders/register_locations_pixel_shader.h"

cbuffer PerObject : register(PER_OBJECT_PS_HLSL)
{
    float timeMs;
    float deltaTimeMs;
    float2 resolution;
    float2 mouse;
};

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
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
