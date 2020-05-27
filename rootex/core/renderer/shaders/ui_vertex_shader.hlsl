#include "register_locations_vertex_shader.h"

cbuffer CBuf : register(PER_OBJECT_VS_HLSL)
{
    matrix M;
};

struct UIVertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float4 color : COLOR;
};

struct UIPixelInput
{
    float4 screenPosition : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 color : COLOR;
};

UIPixelInput main(UIVertexInputType input)
{
    UIPixelInput output;
    output.screenPosition = mul(input.position, M);
    output.tex = input.tex;
    output.color = input.color;
    
    return output;
}
