#include "register_locations_vertex_shader.h"

struct PointLightInfo
{
    float4 ambientColor;
    float4 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
    float3 lightPos;
    float range;
};

struct DirectionalLightInfo
{
    float3 direction;
    float diffuseIntensity;
    float4 ambientColor;
    float4 diffuseColor;
};

struct SpotLightInfo
{
    float4 ambientColor;
    float4 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
    float3 lightPos;
    float range;
    float3 direction;
    float spot;
    float angleRange;
};

struct BasicMaterial
{
    float4 color;
    int isLit;
    float specularIntensity;
    float specPow;
    float reflectivity;
    float refractionConstant;
    float refractivity;
    int affectedBySky;
    int hasNormalMap;
    float fresnelPower;
    float fresnelBrightness;
};

cbuffer CBuf : register(PER_OBJECT_VS_HLSL)
{
    matrix M;
    matrix MInverseTranspose;
    int hasNormalMap;
};

cbuffer CBuf : register(PER_FRAME_VS_HLSL)
{
    matrix V;
    float fogStart;
    float fogEnd;
    // Lighting data
    float3 cameraPos;
    int pointLightCount;
    PointLightInfo pointLightInfos[MAX_DYNAMIC_POINT_LIGHTS];
    int directionLightPresent;
    DirectionalLightInfo directionalLightInfo;
    int spotLightCount;
    SpotLightInfo spotLightInfos[MAX_DYNAMIC_SPOT_LIGHTS];
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
    float3 worldPosition : POSITION0;
    float2 tex : TEXCOORD0;
    float fogFactor : FOG;
    float3 tangent : TANGENT;
    float4 color : COLOR;
    // light position and count
    float3 cameraPos : POSITION1;
    float3 pointLightPos[MAX_DYNAMIC_POINT_LIGHTS] : POSITION2;
    float3 spotLightPos[MAX_DYNAMIC_SPOT_LIGHTS] : POSITION6;
    float3 direction : TEXCOORD1;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    // defining output
    output.screenPosition = mul(input.position, mul(M, mul(V, P)));
    output.normal = normalize(mul((float3)input.normal, (float3x3)MInverseTranspose));
    output.worldPosition = mul(input.position, M).xyz;
    output.tex.x = input.tex.x;
    output.tex.y = 1 - input.tex.y;
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);

    output.tangent = mul(input.tangent, (float3x3)M);

    float4 cameraPosition = mul(input.position, mul(M, V));
    output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));

    output.cameraPos = cameraPos;
    output.direction = directionalLightInfo.direction;

    if (pointLightCount >= 1)
        output.pointLightPos[0] = pointLightInfos[0].lightPos;
    if (pointLightCount >= 2)
        output.pointLightPos[1] = pointLightInfos[1].lightPos;
    if (pointLightCount >= 3)
        output.pointLightPos[2] = pointLightInfos[2].lightPos;
    if (pointLightCount >= 4)
        output.pointLightPos[3] = pointLightInfos[3].lightPos;

    if (spotLightCount >= 1)
        output.spotLightPos[0] = spotLightInfos[0].lightPos;
    if (spotLightCount >= 2)
        output.spotLightPos[1] = spotLightInfos[1].lightPos;
    if (spotLightCount >= 3)
        output.spotLightPos[2] = spotLightInfos[2].lightPos;
    if (spotLightCount >= 4)
        output.spotLightPos[3] = spotLightInfos[3].lightPos;

    // apply transformation in case of Normal Map
    if (hasNormalMap)
    {
        // calculating Transformation Matrix
        float3 N = output.normal;
        float3 T = normalize(output.tangent - dot(output.tangent, N) * N);
        float3 B = cross(N, T);

        float3x3 TBN = float3x3(T, B, N);
        float3x3 TBN_inverse = transpose(TBN);

        // applying transformations
        if (pointLightCount >= 1)
	        output.pointLightPos[0] = mul(pointLightInfos[0].lightPos, TBN_inverse);
        if (pointLightCount >= 2)
	        output.pointLightPos[1] = mul(pointLightInfos[1].lightPos, TBN_inverse);
        if (pointLightCount >= 3)
	        output.pointLightPos[2] = mul(pointLightInfos[2].lightPos, TBN_inverse);
        if (pointLightCount >= 4)
	        output.pointLightPos[3] = mul(pointLightInfos[3].lightPos, TBN_inverse);

        if (spotLightCount >= 1)
	        output.spotLightPos[0] = mul(spotLightInfos[0].lightPos, TBN_inverse);
        if (spotLightCount >= 2)
	        output.spotLightPos[1] = mul(spotLightInfos[1].lightPos, TBN_inverse);
        if (spotLightCount >= 3)
	        output.spotLightPos[2] = mul(spotLightInfos[2].lightPos, TBN_inverse);
        if (spotLightCount >= 4)
	        output.spotLightPos[3] = mul(spotLightInfos[3].lightPos, TBN_inverse);

        output.direction = mul(directionalLightInfo.direction, TBN_inverse);
        output.worldPosition = mul(output.worldPosition, TBN_inverse);
        output.cameraPos = mul(output.cameraPos, TBN_inverse);
    }

    return output;
}
