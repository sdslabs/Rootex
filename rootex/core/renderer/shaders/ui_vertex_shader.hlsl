#include "register_locations_vertex_shader.h"

cbuffer CBuf : register(PER_OBJECT_VS_HLSL)
{
    matrix M;
};

struct UIVertexInputType
{
    float2 position : POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
};

struct UIPixelInput
{
    float4 screenPosition : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
};

UIPixelInput main(UIVertexInputType input)
{
    UIPixelInput output;
    output.screenPosition = mul(float4(input.position, 0.0f, 1.0f), M);
    output.screenPosition.x -= 1.0f;
    output.screenPosition.y -= 1.0f;
    output.screenPosition.y *= -1;
    output.color = input.color;
    output.tex.x = input.tex.x;
    output.tex.y = input.tex.y;
    
    return output;
}
