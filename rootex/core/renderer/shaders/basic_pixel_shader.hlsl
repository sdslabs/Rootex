#include "register_locations_pixel_shader.h"
#include "light.hlsli"
#include "basic_material.hlsli"
#include "sky.hlsli"

Texture2D ShaderTexture : register(DIFFUSE_PS_HLSL);
TextureCube SkyTexture : register(SKY_PS_HLSL);
Texture2D NormalTexture : register(NORMAL_PS_HLSL);
Texture2D SpecularTexture : register(SPECULAR_PS_HLSL);

SamplerState SampleType;

struct PixelInputType
{
    float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    float4 worldPosition : POSITION;
	float2 tex : TEXCOORD0;
	float fogFactor : FOG;
	float3 tangent : TANGENT;
};

cbuffer CBuf : register(PER_OBJECT_PS_HLSL)
{
    BasicMaterial material;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 materialColor = ShaderTexture.Sample(SampleType, input.tex) * material.color;
    float4 finalColor = materialColor;
    
    clip(finalColor.a - 0.00001f);
    
    float3 toEye = normalize(cameraPos - (float3) input.worldPosition);
    
    if (material.isLit)
    {
        finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
        input.normal = normalize(input.normal);

        if (material.hasNormalMap)
		{
			float3 normalMapSample = NormalTexture.Sample(SampleType, input.tex).rgb;
			float3 uncompressedNormal = 2.0f * normalMapSample - 1.0f;
			float3 N = input.normal;
			float3 T = normalize(input.tangent - dot(input.tangent, N) * N);
			float3 B = cross(N, T);

            float3x3 TBN = float3x3(T, B, N);

            input.normal = mul(uncompressedNormal, TBN);
		}

        float3 specularColor = SpecularTexture.Sample(SampleType, input.tex).rgb;
        for (int i = 0; i < pointLightCount; i++)
        {
            finalColor += saturate(GetColorFromPointLight(pointLightInfos[i], toEye, input.normal, input.worldPosition, materialColor, specularColor, material));
        }
    
        for (i = 0; i < MAX_STATIC_POINT_LIGHTS_AFFECTING_1_OBJECT; i++)
        {
            if (material.staticPointsLightsAffecting[i] == -1)
            {
                break;
            }
            finalColor += saturate(GetColorFromPointLight(staticPointLightInfos[material.staticPointsLightsAffecting[i]], toEye, input.normal, input.worldPosition, materialColor, specularColor, material));
        }
        
        if (directionLightPresent == 1)
        {
            finalColor += saturate(GetColorFromDirectionalLight(directionalLightInfo, toEye, input.normal, materialColor, specularColor, material));
        }
    
        for (i = 0; i < spotLightCount; i++)
        {
            finalColor += saturate(GetColorFromSpotLight(spotLightInfos[i], toEye, input.normal, input.worldPosition, materialColor, specularColor, material));
        }
    }
    
    if (material.affectedBySky)
    {
        finalColor.rgb = GetReflectionFromSky(finalColor, toEye, input.normal, SkyTexture, SampleType, material);
        finalColor.rgb = GetRefractionFromSky(finalColor, input.normal, input.worldPosition, cameraPos, SkyTexture, SampleType, material);
    }
    
    finalColor.rgb = (lerp(fogColor, finalColor, input.fogFactor)).rgb;

    return finalColor;
}
