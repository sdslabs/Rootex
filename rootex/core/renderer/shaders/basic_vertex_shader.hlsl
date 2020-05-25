#include "register_locations_vertex_shader.h"

cbuffer CBuf : register(PER_OBJECT_VS_HLSL)
{
    matrix M;
    matrix MInverseTranspose;
};

cbuffer CBuf : register(PER_FRAME_VS_HLSL)
{
    matrix V;
};

cbuffer CBuf : register(PER_CAMERA_CHANGE_VS_HLSL)
{
    matrix P;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float4 normal : NORMAL;
};

struct PixelInputType
{
    float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    float4 worldPosition : POSITION;
    float2 tex : TEXCOORD0;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    output.screenPosition = mul(input.position, mul(M, mul(V, P)));
    //inverse transpose is needed for normals, how is this even working...
    //output.normal = mul((float3x3) M, (float3) input.normal);
    //hold my beer...
    output.normal = mul((float3) input.normal, (float3x3) MInverseTranspose);
    output.worldPosition = mul(input.position, M);
    output.tex.x = input.tex.x;
    output.tex.y = 1 - input.tex.y;
    input.normal = normalize(input.normal);
    
    return output;
}
