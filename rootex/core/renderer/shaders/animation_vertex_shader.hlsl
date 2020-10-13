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
	matrix FinalTransforms[256];
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
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;
		
    float3 pos = float3(0.0f, 0.0f, 0.0f);

    pos += input.weights.x * mul(input.position, FinalTransforms[input.indices.x]).xyz;
    pos += input.weights.y * mul(input.position, FinalTransforms[input.indices.y]).xyz;
    pos += input.weights.z * mul(input.position, FinalTransforms[input.indices.z]).xyz;
    pos += input.weights.w * mul(input.position, FinalTransforms[input.indices.w]).xyz;
	
    output.screenPosition = mul(float4(pos, 1.0f), mul(M, mul(V, P)));
    output.normal = normalize(mul((float3) input.normal, (float3x3) MInverseTranspose));
    
	output.tex.x = input.tex.x;
    output.tex.y = input.tex.y;
	
    output.tangent = mul(input.tangent, M);
	
    output.worldPosition = mul(input.position, M);
    float4 cameraPosition = mul(input.position, mul(M, V));
    output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));
	return output;
}
