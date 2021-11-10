#include "register_locations_pixel_shader.h"

Texture2D ShaderTexture : register(DIFFUSE_PS_HLSL);
Texture2D NormalTexture : register(NORMAL_PS_HLSL);
Texture2D SpecularTexture : register(SPECULAR_PS_HLSL);
Texture2D LightmapTexture : register(LIGHTMAP_PS_HLSL);
Texture2D DepthTexture : register(DEPTH_TEXTURE_PS_HLSL);
SamplerState SampleType : register(SAMPLER_PS_HLSL);

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

cbuffer CBuf : register(PER_OBJECT_PS_HLSL)
{
	BasicMaterial material;
};

cbuffer CBuf : register(PER_MODEL_PS_HLSL)
{
	int staticPointLightAffectingCount = 0;
	int staticPointsLightsAffecting[MAX_STATIC_POINT_LIGHTS_AFFECTING_1_OBJECT];
};

float4 main(PixelInputType input)
    : SV_TARGET
{
	float4 materialColor = ShaderTexture.Sample(SampleType, input.tex) * material.color * input.color;
	float4 finalColor = materialColor;

	clip(finalColor.a - 0.0001f);

	float3 toEye = normalize(cameraPos - (float3)input.worldPosition);

	finalColor.rgb = lerp(finalColor.rgb, float3(0.0f, 0.0f, 0.0f), material.isLit);
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
		finalColor += saturate(GetColorFromPointLight(pointLightInfos[i], toEye, input.normal, input.worldPosition, materialColor, specularColor, material.specPow, material.specularIntensity, material.isLit));
	}

	for (i = 0; i < staticPointLightAffectingCount; i++)
	{
		finalColor += saturate(GetColorFromPointLight(staticPointLightInfos[staticPointsLightsAffecting[i]], toEye, input.normal, input.worldPosition, materialColor, specularColor, material.specPow, material.specularIntensity, material.isLit));
	}

	finalColor += saturate(GetColorFromDirectionalLight(directionalLightInfo, toEye, input.normal, materialColor, specularColor, material.specPow, material.specularIntensity, material.isLit));

	for (i = 0; i < spotLightCount; i++)
	{
		finalColor += saturate(GetColorFromSpotLight(spotLightInfos[i], toEye, input.normal, input.worldPosition, materialColor, specularColor, material.specPow, material.specularIntensity, material.isLit));
	}

	finalColor.rgb = GetReflectionFromSky(finalColor, toEye, input.normal, SkyTexture, SampleType, material.reflectivity, material.affectedBySky, material.fresnelPower, material.fresnelBrightness);
	finalColor.rgb = GetRefractionFromSky(finalColor, input.normal, input.worldPosition, cameraPos, SkyTexture, SampleType, material.refractionConstant, material.refractivity, material.affectedBySky);

	finalColor = finalColor * LightmapTexture.Sample(SampleType, input.tex);
	finalColor.rgb = (lerp(fogColor, finalColor, input.fogFactor)).rgb;

	return finalColor;
}
