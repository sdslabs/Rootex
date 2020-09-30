/* #include "register_locations_pixel_shader.h"

Texture2D ShaderTexture;
SamplerState SampleType;
TextureCube SkyTexture :register(SKY_PS_HLSL);

struct PixelInputType
{
	float4 screenPosition : SV_POSITION;
	float3 normal : NORMAL;
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
}; // check plox

cbuffer Material : register(PER_OBJECT_PS_HLSL) 
{
	float4 color;
	int isLit;
	float specularIntensity;
	float specPow;
	float reflectivity;
	float refractionConstant;
	float refractivity;
	int isAffectedBySky;
};

float4 main(PixelInputType input) : SV_POSITION
{

}
*/
#include "register_locations_pixel_shader.h"

Texture2D ShaderTexture : register(DIFFUSE_PS_HLSL);
SamplerState SampleType;
TextureCube SkyTexture : register(SKY_PS_HLSL);
Texture2D NormalTexture : register(NORMAL_PS_HLSL);

struct PixelInputType
{
    float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    float4 worldPosition : POSITION;
    float2 tex : TEXCOORD0;
    float fogFactor : FOG;
    float3 tangent : TANGENT;
};
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

cbuffer Lights : register(PER_FRAME_PS_HLSL)
{
    float3 cameraPos;
    int pointLightCount;
    PointLightInfo pointLightInfos[MAX_POINT_LIGHTS];
    int directionLightPresent;
    DirectionalLightInfo directionalLightInfo;
    int spotLightCount;
    SpotLightInfo spotLightInfos[MAX_SPOT_LIGHTS];
    float4 fogColor;
}

cbuffer Material : register(PER_OBJECT_PS_HLSL)
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
};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 materialColor = ShaderTexture.Sample(SampleType, input.tex) * color;
    float4 finalColor = materialColor;
    float3 toEye = normalize(cameraPos - (float3) input.worldPosition);
    
    if (isLit)
    {
        input.normal = normalize(input.normal);

        if (hasNormalMap)
        {
            float3 normalMapSample = NormalTexture.Sample(SampleType, input.tex).rgb;
            float3 uncompressedNormal = 2.0f * normalMapSample - 1.0f;
            float3 N = input.normal;
            float3 T = normalize(input.tangent - dot(input.tangent, N) * N);
            float3 B = cross(N, T);

            float3x3 TBN = float3x3(T, B, N);

            input.normal = mul(uncompressedNormal, TBN);
        }

        for (int i = 0; i < pointLightCount; i++)
        {
            float3 relative = pointLightInfos[i].lightPos - (float3) input.worldPosition;
            float dist = length(relative);
            if (dist <= pointLightInfos[i].range)
            {
                float3 normalizedRelative = relative / dist;
                float att = 1.0f / (pointLightInfos[i].attConst + pointLightInfos[i].attLin * dist + pointLightInfos[i].attQuad * (dist * dist));
                float cosAngle = max(0.0f, dot(normalizedRelative, input.normal));
                float3 diffuse = pointLightInfos[i].diffuseColor * pointLightInfos[i].diffuseIntensity * cosAngle;
                float3 reflected = reflect(-normalizedRelative, input.normal);
                float specFactor = pow(max(dot(normalize(reflected), toEye), 0.0f), specPow);
                float3 specular = specularIntensity * specFactor * diffuse;
        
                finalColor += float4(saturate(((diffuse + (float3) pointLightInfos[i].ambientColor) * (float3) materialColor + specular) * att), 0.0f);
            }
        }
    
        if (directionLightPresent == 1)
        {
            float3 direction = normalize(directionalLightInfo.direction);
            float cosAngle = max(0.0f, dot(-direction, input.normal));
            float3 diffuse = pointLightInfos[i].diffuseColor * directionalLightInfo.diffuseIntensity * cosAngle;
            float3 reflected = reflect(-direction, input.normal);
            float specFactor = pow(max(dot(normalize(reflected), toEye), 0.0f), specPow);
            float3 specular = specularIntensity * specFactor * diffuse;
            finalColor += float4(saturate((diffuse + (float3) directionalLightInfo.ambientColor) * (float3) materialColor + specular), 0.0f);
        }
    
        for (i = 0; i < spotLightCount; i++)
        {
            float3 relative = spotLightInfos[i].lightPos - (float3) input.worldPosition;
            float dist = length(relative);
            if (dist <= spotLightInfos[i].range)
            {
                float3 normalizedRelative = relative / dist;
                float cosAngle = max(0.0f, dot(normalizedRelative, input.normal));
                float rangeAngle = max(dot(-normalizedRelative, spotLightInfos[i].direction), 0.0f);
                if (rangeAngle > spotLightInfos[i].angleRange)
                {
                    float att = 1.0f / (spotLightInfos[i].attConst + spotLightInfos[i].attLin * dist + spotLightInfos[i].attQuad * (dist * dist));
                    float3 diffuse = spotLightInfos[i].diffuseColor * spotLightInfos[i].diffuseIntensity * cosAngle;
                    float3 reflected = reflect(-normalizedRelative, input.normal);
                    float specFactor = pow(max(dot(normalize(reflected), toEye), 0.0f), specPow);
                    float3 specular = specularIntensity * specFactor * diffuse;
            
                    float spotFactor = pow(rangeAngle, spotLightInfos[i].spot);
        
                    finalColor += float4(saturate(((diffuse + (float3) spotLightInfos[i].ambientColor) * (float3) materialColor + specular) * att * spotFactor), 0.0f);
                }
            }
        }
    }
    
    if (affectedBySky)
    {
        float3 incident = -toEye;
        float3 reflectionVector = reflect(incident, input.normal);
        float4 reflectionColor = SkyTexture.Sample(SampleType, reflectionVector);
        finalColor.rgb = lerp(finalColor, reflectionColor, reflectivity);
    
        float3 refractionIncident = normalize(input.worldPosition.xyz - cameraPos);
        float3 refractionReflect = refract(refractionIncident, normalize(input.normal), refractionConstant);
        float4 refractionColor = SkyTexture.Sample(SampleType, refractionReflect);
        finalColor.rgb = lerp(finalColor, refractionColor, refractivity);
    }
    
    finalColor.rgb = lerp(fogColor, finalColor, input.fogFactor);

    return finalColor;
}
