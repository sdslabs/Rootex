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

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float4 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct DecalPixelInputType
{
	float4 screenPosition : SV_POSITION;
	float3 normal : NORMAL;
	float4 worldPosition : POSITION;
	float2 tex : TEXCOORD0;
	float fogFactor : FOG;
	float3 tangent : TANGENT;
	float4 color : COLOR;
	float3 decalRight : TEXCOORD1;
	float3 decalForward : TEXCOORD2;
	float3 decalUp : TEXCOORD3;
	float3 decalHalfScale : TEXCOORD4;
};

DecalPixelInputType main(VertexInputType input)
{
	DecalPixelInputType output;

	output.screenPosition = mul(input.position, mul(M, mul(V, P)));
	output.tex.x = input.tex.x;
	output.tex.y = 1 - input.tex.y;
	output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	output.worldPosition = mul(float4(0.0f, 0.0f, 0.0f, 1.0f), M);

	float3 decalRight = mul(float4(1.0f, 0.0f, 0.0f, 1.0f), M).xyz - output.worldPosition.xyz;
	float3 decalForward = mul(float4(0.0f, 0.0f, -1.0f, 1.0f), M).xyz - output.worldPosition.xyz;
	float3 decalUp = mul(float4(0.0f, 1.0f, 0.0f, 1.0f), M).xyz - output.worldPosition.xyz;
	output.decalHalfScale = float3(length(decalRight), length(decalRight), length(decalUp)) / 2.0f;

	output.normal = normalize(mul(float3(0.0f, 0.0f, 1.0f), (float3x3)MInverseTranspose));
	output.tangent = mul(float3(1.0f, 0.0f, 0.0f), (float3x3)M);

	float4 cameraPosition = mul(input.position, mul(M, V));
	output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));

	return output;
}
