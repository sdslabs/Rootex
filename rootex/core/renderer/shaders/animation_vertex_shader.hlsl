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

cbuffer CBuf : register(BONES)
{
    matrix BoneTransforms[96];
}

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float4 normal : NORMAL;
    float4 boneWeights : BONEWEIGHTS;
    uint boneIndices[4] : BONEINDICES;
};

float4 main(VertexInputType input) : SV_POSITION
{
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = input.boneWeights.x;
    weights[1] = input.boneWeights.y;
    weights[2] = input.boneWeights.z;
    weights[3] = input.boneWeights.w;
    
    float3 posL = float3(0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < 4; ++i)
    {
        posL += weights[i] * mul(input.position, BoneTransforms[input.boneIndices[i]]).xyz;
    }
        
    return mul(float4(posL, 1.0f), mul(M, mul(V, P)));
}
