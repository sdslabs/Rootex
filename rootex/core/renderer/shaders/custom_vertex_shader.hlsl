#include "rootex/core/renderer/shaders/register_locations_vertex_shader.h"

cbuffer CBuf : register(PER_OBJECT_VS_HLSL)
{
    matrix M;
    matrix MInverseTranspose;
};

cbuffer CBuf : register(PER_FRAME_DL_VS_HLSL)
{
    matrix DLTC;
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

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float4 normal : NORMAL;
	float3 tangent : TANGENT;
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
    output.screenPosition = mul(input.position, mul(M, mul(V, P)));
	output.normal = normalize(mul((float3)input.normal, (float3x3)MInverseTranspose));
    output.worldPosition = mul(input.position, M);
    output.tex.x = input.tex.x;
    output.tex.y = 1 - input.tex.y;
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    output.tangent = mul(input.tangent, (float3x3)M);
	
    float4 cameraPosition = mul(input.position, mul(M, V));
    output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));
	
	return output;
}
