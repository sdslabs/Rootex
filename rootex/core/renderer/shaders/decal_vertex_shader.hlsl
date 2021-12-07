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
	float3 tangent : TANGENT;
	float4 color : COLOR;
	nointerpolation float3 decalRight : TEXCOORD1;
	nointerpolation float3 decalForward : TEXCOORD2;
	nointerpolation float3 decalUp : TEXCOORD3;
	nointerpolation float3 decalHalfScale : TEXCOORD4;
	nointerpolation float4 decalViewspacePosition : TEXCOORD5;
};

DecalPixelInputType main(VertexInputType input)
{
	DecalPixelInputType output;
	output.screenPosition = mul(input.position, mul(M, mul(V, P)));
	output.tex.x = input.tex.x;
	output.tex.y = 1 - input.tex.y;
	output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	output.worldPosition = mul(float4(0.0f, 0.0f, 0.0f, 1.0f), M);

	float4x4 MV = mul(M, V);

	output.decalViewspacePosition = mul(float4(0.0f, 0.0f, 0.0f, 1.0f), MV);

	output.decalRight = mul(float4(1.0f, 0.0f, 0.0f, 1.0f), MV).xyz - output.decalViewspacePosition.xyz;
	output.decalForward = mul(float4(0.0f, 0.0f, -1.0f, 1.0f), MV).xyz - output.decalViewspacePosition.xyz;
	output.decalUp = mul(float4(0.0f, 1.0f, 0.0f, 1.0f), MV).xyz - output.decalViewspacePosition.xyz;
	output.decalHalfScale = float3(length(output.decalRight), length(output.decalForward), length(output.decalUp)) / 2.0f;

	output.decalRight = normalize(output.decalRight);
	output.decalForward = normalize(output.decalForward);
	output.decalUp = normalize(output.decalUp);

	output.normal = normalize(mul(float3(0.0f, 0.0f, 1.0f), (float3x3)MInverseTranspose));
	output.tangent = mul(float3(1.0f, 0.0f, 0.0f), (float3x3)M);

	float4 cameraPosition = mul(input.position, mul(M, V));

	return output;
}
