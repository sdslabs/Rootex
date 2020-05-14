#include "register_locations_pixel_shader.h"

cbuffer CBuf : register(PER_OBJECT_PS_HLSL)
{
    float4 color;
};

float4 main() : SV_TARGET
{
    return color;
}
