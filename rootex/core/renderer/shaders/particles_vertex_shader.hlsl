#include "register_locations_vertex_shader.h"

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
    float4 instanceRowX : INSTANCE_ROWX;
    float4 instanceRowY : INSTANCE_ROWY;
    float4 instanceRowZ : INSTANCE_ROWZ;
    float4 instanceRowW : INSTANCE_ROWW;
    float4 instanceInverseTransposeRowX : INSTANCE_INVERSE_TRANSPOSE_ROWX;
    float4 instanceInverseTransposeRowY : INSTANCE_INVERSE_TRANSPOSE_ROWY;
    float4 instanceInverseTransposeRowZ : INSTANCE_INVERSE_TRANSPOSE_ROWZ;
    float4 instanceInverseTransposeRowW : INSTANCE_INVERSE_TRANSPOSE_ROWW;
    float4 instanceColor : INSTANCE_COLOR;
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
    float4x4 model = float4x4(input.instanceRowX, input.instanceRowY, input.instanceRowZ, input.instanceRowW);
    float4x4 modelInverseTranspose = float4x4(input.instanceInverseTransposeRowX, input.instanceInverseTransposeRowY, input.instanceInverseTransposeRowZ, input.instanceInverseTransposeRowW);
    output.screenPosition = mul(input.position, mul(model, mul(V, P)));
    output.normal = normalize(mul((float3) input.normal, (float3x3) modelInverseTranspose));
    output.worldPosition = mul(input.position, model);
    output.tex.x = input.tex.x;
    output.tex.y = 1 - input.tex.y;
    output.tangent = mul(input.tangent, (float3x3) model);
    output.color = input.instanceColor;
    
    float4 cameraPosition = mul(input.position, mul(model, V));
    output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));
	
    return output;
}
