#include "register_locations_vertex_shader.h"

cbuffer CBuf : register(PER_OBJECT_VS_HLSL)
{
	matrix M;
	matrix MInverseTranspose;
};

cbuffer CBuf : register(PER_FRAME_VS_HLSL)
{
	matrix V;
	float fogStart;
	float fogEnd;
};

cbuffer CBuf : register(PER_CAMERA_CHANGE_VS_HLSL)
{
	matrix P;
};

cbuffer CBuf : register(BONES_VS_HLSL)
{
	matrix FinalTransforms[MAX_BONES];
}

struct VertexInputType
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 tex : TEXCOORD0;
    float3 tangent : TANGENT;
	int4 indices : BONEINDICES;
	float4 weights : BONEWEIGHTS;
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

PixelInputType main(VertexInputType input)
{
	PixelInputType output;
    
    matrix transform = mul(input.weights.x, FinalTransforms[input.indices.x]);
    transform += mul(input.weights.y, FinalTransforms[input.indices.y]);
    transform += mul(input.weights.z, FinalTransforms[input.indices.z]);
    transform += mul(input.weights.w, FinalTransforms[input.indices.w]);
    
    float3 pos = mul(input.position, transform).xyz;
    output.screenPosition = mul(float4(pos, 1.0f), mul(M, mul(V, P)));
    
    float3 nor = normalize(mul(input.normal, transform).xyz);
    
    output.normal = normalize(mul((float3) nor, (float3x3) MInverseTranspose));
	
    output.tex.x = input.tex.x;
    output.tex.y = input.tex.y;
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	
    output.tangent = mul(input.tangent, (float3x3)M);
	
    output.worldPosition = mul(float4(pos, 1.0f), M);
    float4 cameraPosition = mul(float4(pos, 1.0f), mul(M, V));
    output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));
	
	return output;
}
