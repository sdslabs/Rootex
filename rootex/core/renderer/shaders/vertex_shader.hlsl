#include "register_locations_vertex_shader.h"

cbuffer CBuf : register(PER_OBJECT)
{
    matrix M;
};

cbuffer CBuf : register(PER_FRAME)
{
    matrix V;
};

cbuffer CBuf : register(PER_CAMERA_CHANGE)
{
    matrix P;
};

float4 main(float3 pos : POSITION) : SV_POSITION
{
    return mul(float4(pos, 1.0f), mul(M, mul(V, P)));
}
