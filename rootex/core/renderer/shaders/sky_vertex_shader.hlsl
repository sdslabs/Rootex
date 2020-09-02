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
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD0;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    output.position = mul(input.position, mul(M, mul(V, P))).xyww;
    output.texCoord = input.position;
    
    return output;
}
